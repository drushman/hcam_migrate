<?php

class LanggoMigrateI18nSettingsMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate i18n Settings';

    // Mapping
    $map_dest = $map_source = MigrateDestinationTable::getKeySchema('variable');
    $this->map = new MigrateSQLMap($this->machineName, $map_source, $map_dest);

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('variable', 'variable');
    $select->fields('variable');
    $select->condition('name', $this->listVar(), 'IN');
    $this->source = new MigrateSourceSQL($select);

    // Destination
    db_set_active();
    $this->destination = new HCUserSettingsDestinationTable('variable');

    // Field Mapping
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('value', 'value');
  }
  public function listVar() {
    $list = array(
      'i18n_hide_translation_links',
    );
    return $list;
  }

}



/**
 * Migrate Content Type Language settings.
 */
class LanggoMigrateI18nMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate Budle language setting';

    // Mapping
    $map_dest = $map_source = MigrateDestinationI18nVariable::getKeySchema();
    $this->map = new MigrateSQLMap($this->machineName, $map_source, $map_dest);

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    //get 
    
    $select = db_select('node_type', 'n');
    $select->fields('n', array('type'));
    $this->source = new MigrateSourceSQL($select);

    // Destination
    db_set_active();
    $this->destination = new MigrateDestinationI18nVariable();

    // Field Mapping
    $this->addFieldMapping('type', 'type');
  }

}


/**
 * 
 */


class MigrateDestinationI18nVariable extends MigrateDestinationEntity {

  /**
   * An array with content ids of imported messages. Not yet used.
   */
  var $importedIds = array();

  static public function getKeySchema() {
    return array(
      'type' => array(
        'type' => 'varchar',
        'length' => 32,
        'not null' => TRUE,
      ),
    );
  }

  /**
   * Basic initialization
   *
   * @param array $options
   */
  public function __construct(array $options = array()) {
    parent::__construct('variables_i18n', 'variables_i18n', $options);
  }

  /**
   * Returns a list of fields available to be mapped/
   *
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
    $fields = array(
        'type' => 'Type',
    );

    // Then add in anything provided by handlers
    $fields += migrate_handler_invoke_all('Entity', 'fields', $this->entityType, $this->bundle);
    $fields += migrate_handler_invoke_all('PrivateMsg', 'fields', $this->entityType, $this->bundle);

    return $fields;
  }
  /**
   * 
   * @param type $type
   * @return string
   * get list variable in drupal 6
   */
  public function names($type) {
    $names = array(
      'i18n_sync_node_type_' . $type,
      'language_content_type_' . $type,
      'i18n_newnode_current_' . $type,
      'i18n_required_node_' . $type,
      'i18n_lock_node_' . $type,
      'i18n_node_' . $type,
    );
    return $names;
  }
  /**
   * 
   * @param type $type
   * @return string
   * get list variables in drupal 7
   */
  public function names7($type) {
    $names = array(
      'i18n_node_options_' . $type,
      'language_content_type_' . $type,
    );
    return $names;
  }
  /**
   * Mark provided message as deleted.
   *
   * @param $id
   *  IDs to be deleted.
   */
  public function rollback(array $id) {
    migrate_instrument_start(__METHOD__);
    // Delete variables of the content type.
    db_delete('variable')
      ->condition('name', $this->names7(reset($id)), 'IN')
      ->execute();
    migrate_instrument_stop(__METHOD__);
  }

  /**
   * Import a single message.
   *
   * @param $entity
   *  Object object to build. Prefilled with any fields mapped in the Migration.
   * @param $row
   *  Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *  Array of key fields of the object that was saved if
   *  successful. FALSE on failure.
   */
  public function import(stdClass $entity, stdClass $row) {
    $this->prepare($entity, $row);
    $type = $entity->type;
    
    //get variables content type
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_query('SELECT name, value FROM {variable} WHERE name IN (:names)', array(':names' => $this->names($type)));
    $variables = array_map('unserialize', $query->fetchAllKeyed());
    
    //set variables
    db_set_active();
    $this->setVariables($variables, $type);
    $this->numCreated++;
    $this->complete($entity, $row);
    return array($type);    
  }
  /**
   * 
   * @param type $variables
   * set variables
   */
  public function setVariables($variables, $type) {
    $option = array();
    foreach( $variables as $name => $value) {
      switch ($name) {
        case 'i18nsync_nodeapi_' . $type:
          variable_set('i18n_sync_node_type_' . $type, $value);
          break;
        case 'language_content_type_' . $type:
          variable_set('language_content_type_' . $type, $value);
          break;
        case 'i18n_node_' . $type:
          variable_set('i18n_node_extended_' . $type, $value);
          break;
        case 'i18n_newnode_current_' . $type:
          if ($value != 1)            break;
          $option[] = 'current'; 
          break;
        case 'i18n_required_node_' . $type:
          if ($value != 1) break;
          $option[] = 'required'; 
          break;
        case 'i18n_lock_node_' . $type:
          if ($value != 1) break;
          $option[] = 'lock'; 
          break;
      }
      variable_set('i18n_node_options_' . $type, $option);
    }
  }

}

