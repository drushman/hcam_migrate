<?php
/**
 * @file langgo_migrate_node.base.field.inc
 */

/**
 * Base class to migrate node fields.
 *
 * Must resolve in sub class:
 *
 *    1. d7.type
 *    2. d7.data (d6.db_columns)
 *
 * Check later:
 *
 *    1. d6.required - Migrate in FieldInstance
 *
 */
abstract class LanggoNodeFieldMigrateBase extends LanggoMigrateBase {
  protected $field_type;

  public function __construct() {
    parent::__construct();
    $this->description = "Migrate migrate node.field.{$this->field_type}";
    $this->dependencies[] = 'HCNodeTypes';

    // Key Mapping
    $s_keys['field_name'] = array('type' => 'varchar', 'length' => 32);
    $d_keys = MigrateDestinationTable::getKeySchema('field_config');
    $this->map = new MigrateSQLMap($this->machineName, $s_keys, $d_keys);

    // Source
    $this->source = new MigrateSourceSQL($this->query());
    $this->destination = new MigrateDestinationTable('field_config');
    $this->mapping();
  }

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('content_node_field', 'field');
    $query->condition('type', $this->field_type);
    $query->fields('field');
    db_set_active();
    return $query;
  }

  protected function mapping() {
    $this->addFieldMapping('field_name', 'field_name');
    $this->addFieldMapping('type', 'type');
    $this->addFieldMapping('data', 'global_settings');
    $this->addFieldMapping('storage_active', 'db_storage');
    $this->addFieldMapping('module', 'module');
    $this->addFieldMapping('storage_type', NULL)->defaultValue('field_sql_storage');
    $this->addFieldMapping('storage_module', NULL)->defaultValue('field_sql_storage');
    $this->addFieldMapping('translatable', NULL)->defaultValue(0);
    $this->addFieldMapping('cardinality', 'multiple');
    $this->addFieldMapping('deleted', NULL)->defaultValue(0);
    $this->addFieldMapping('active', 'active');
    $this->addFieldMapping('locked', 'locked');
    $this->addUnmigratedSources(array('required', 'db_columns'));
    $this->addUnmigratedDestinations(array('id'));
  }

  /**
   * Generate data column for table in Drupal 7.
   */
  public function prepare($field, $row) {
    // Old: multiple
    //  0: 1 value
    //  1: unlimited
    //  2: 2
    // New:
    //  -1: unlimited
    //   1: 1
    switch ($field->cardinality) {
      case 0:
        $field->cardinality = 1;
        break;
      case 1:
        $field->cardinality = -1;
        break;
    }

    $field->data = array();
    $field->data['entity_types'] = array();
    $field->data['translatable'] = 0;
    $field->data['settings'] = array();
    $field->data['storage'] = array(
      'type' => 'field_sql_storage',
      'settings' => array(),
      'module' => 'field_sql_storage',
      'active' => 1,
    );
  }

  /**
   * Call Drupal API to create new field.
   */
  protected function createField($field) {
    $field = (array)($field);
    $field = field_create_field($field);
    $field = (object)($field);
  }
}
