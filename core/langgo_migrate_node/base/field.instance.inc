<?php
/**
 * @file langgo_migrate_node.base.field.instance.inc
 */

/**
 * Base class to migrate field instances.
 */
abstract class LanggoNodeFieldInstanceMigrationBase extends LanggoMigrateBase {
  protected $source_migration;
  protected $widget_module;
  protected $widget_type;

  public function __construct() {
    parent::__construct();
    $this->description = 'Migrate Node Fields';
    $this->dependencies[] = 'HCNodeTypes';
    $this->dependencies[] = $this->source_migration;

    // Key Mapping
    $s_keys['field_name'] = array('type' => 'varchar', 'length' => 32, 'alias' => 'fi');
    $s_keys['type_name']  = array('type' => 'varchar', 'length' => 32);
    $d_keys = MigrateDestinationTable::getKeySchema('field_config_instance');
    $this->map = new MigrateSQLMap($this->machineName, $s_keys, $d_keys);

    // Other setups
    $this->source = new MigrateSourceSQL($query = $this->query());
    $this->destination = new MigrateDestinationTable('field_config_instance');
    $this->mappings();
  }

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('content_node_field_instance', 'fi');
    $query->innerJoin('content_node_field', 'f', 'f.field_name = fi.field_name');
    $query->condition('fi.widget_module',   $this->widget_module);
    $query->condition('fi.widget_type',     $this->widget_type);
    $query->fields('fi', array('field_name', 'type_name', 'weight', 'label', 'widget_type', 'widget_settings', 'display_settings', 'description', 'widget_module', 'widget_active'));
    $query->addField('f', 'required');
    $query->addField('f', 'global_settings');
    $query->orderBy('weight');
    db_set_active();
    return $query;
  }

  protected function mappings() {
    $this->addFieldMapping('field_id', 'field_name')
        ->sourceMigration(array($this->source_migration));
    $this->addFieldMapping('field_name', 'field_name');
    $this->addFieldMapping('entity_type', NULL)->defaultValue('node');
    $this->addFieldMapping('bundle', 'type_name');
    $this->addFieldMapping('deleted', NULL)->defaultValue(0);
    $this->addUnmigratedDestinations(array('id', 'data'));
    $this->addUnmigratedSources(array('label', 'description', 'weight', 'required'));
    $this->addUnmigratedSources(array('display_settings', 'global_settings'));
    $this->addUnmigratedSources(array('widget_module', 'widget_active', 'widget_type', 'widget_settings'));
  }

  /**
   * Sub classes should resolve these values:
   *  1. $entity['widget']['type']
   *  2. $entity['widget']['module'].
   */
  public function prepare($entity, $row) {
    $row->widget_settings = unserialize($row->widget_settings);
    $row->display_settings = unserialize($row->display_settings);
    $row->global_settings = unserialize($row->global_settings);

    $entity->data = array();
    $entity->data['label'] = $row->label;
    $entity->data['description'] = $row->description;
    $entity->data['required'] = $row->required;
    $entity->data['widget']['weight'] = $row->weight;
    $entity->data['widget']['active'] = $row->widget_active;
    $entity->data['settings']['user_register_form'] = FALSE;
  }

  /**
   * Call Drupal API to create field instance.
   * 
   * @param type $instance
   */
  protected function createInstance($instance) {
    $instance = (array)($instance);
    $instance = field_create_instance($instance);
    $instance = (object)($instance);
  }
}
