<?php
/**
 * @file langgo_migrate_node.type.inc
 *
 * Migrate node.types from Drupal 6 to 7.
 *
 * 1. Types
 * 2. Settings
 *      Permissions
 * 3. Fields
 * 4. Displays
 */

/**
 * Migrate node types.
 */
class HCNodeTypesMigration extends LanggoMigrateBase {
  protected $exclude_types    = array();

  public function __construct() {
    parent::__construct();
    $this->description = 'Migrate Node Types';
    $this->dependencies[] = 'HCLanguage';
    $this->dependencies[] = 'HCUserRoles';

    // Key Mapping
    $keys = MigrateDestinationTable::getKeySchema('node_type');
    $this->map = new MigrateSQLMap($this->machineName, $keys, $keys);

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('node_type');
    $select->fields('node_type');

    // Exclude some modules
    $select->condition('module', LanggoNodeHelper::getExcludedBundleModules(), 'NOT IN');
    
    if (!empty($this->exclude_types)) {
      $select->condition('type', $this->exclude_types, 'NOT IN');
    }
    $this->source = new MigrateSourceSQL($select);
    db_set_active();

    // Destination
    $this->destination = new MigrateDestinationTable('node_type');

    // Field Mapping
    $this->addFieldMapping('type', 'type');
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('base', NULL)->defaultValue('node_content');
    $this->addFieldMapping('module', 'module');
    $this->addFieldMapping('description', 'description');
    $this->addFieldMapping('help', 'help');
    $this->addFieldMapping('has_title', 'has_title');
    $this->addFieldMapping('title_label', 'title_label');
    $this->addFieldMapping('custom', 'custom');
    $this->addFieldMapping('modified', 'modified');
    $this->addFieldMapping('locked', 'locked');
    $this->addFieldMapping('orig_type', 'orig_type');
    $this->addFieldMapping('disabled', NULL)->defaultValue(0);
    $this->addUnmigratedSources(array('has_body', 'body_label', 'min_word_count'));
  }

  public function prepare($entity, $row) {
    switch ($entity->module) {
      case 'poll':
      case 'blog':
        $entity->base = $entity->module;
        break;
    }

    $find = db_select('node_type');
    $find->fields('node_type', array('type'));
    $find->condition('type', $entity->type);
    if ($find->execute()->fetchColumn()) {
      db_delete('node_type')->condition('type', $entity->type)->execute();
    }
  }

  public function complete($entity, $row) {
    if ($row->has_body) {
      node_add_body_field($entity, $row->body_label);
    }
  }
}
