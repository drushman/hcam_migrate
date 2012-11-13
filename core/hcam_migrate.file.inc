<?php
/**
 * @file langgo_migrate_main.file.inc
 *
 * Migrate file objects from Drupal 6 to Drupal 7.
 *
 * @see system_update_7034() - Create {file_managed}
 */

/**
 * Migrate files to file_managed.
 *
 * @TODO: Check this http://drupalcode.org/sandbox/mikeryan/1234554.git/blob/refs/heads/7.x-2.x:/file.inc
 */
class HCFileMigration extends LanggoMigrateBase {
  public function __construct() {
    parent::__construct();
    $this->description = 'Migrate Files';
    $this->dependencies[] = 'HCUser';

    // Keys Mapping
    $keys = MigrateDestinationTable::getKeySchema('file_managed');
    $this->map = new MigrateSQLMap($this->machineName, $keys, $keys);

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('files');
    $select->fields('files');
    $select->orderBy('fid');
    $this->source = new MigrateSourceSQL($select);
    db_set_active();

    // Destination
    $this->destination = new MigrateDestinationTable('file_managed');

    // Field Mappings
    $this->addFieldMapping('uid', 'uid')->getSourceMigration('HCUser');
    $this->addFieldMapping('filename', 'filename');
    $this->addFieldMapping('filemime', 'filemime');
    $this->addFieldMapping('filesize', 'filesize');
    $this->addFieldMapping('status', 'status');
    $args['dir_path'] = $dir = $this->variableGet('file_directory_path');
    $args['dir_path'] = trim($args['dir_path'], '/');
    $this->addFieldMapping('uri', 'filepath')->arguments($args);
    $this->addFieldMapping('timestamp', 'timestamp');
  }

  /**
   * 1. Set for right user ID.
   * 2. Update the filepath, use new style of Drupal 7 -- public://, private://
   */
  public function prepare(&$entity, $row) {
    $entity->uri = str_replace("{$entity->uri['arguments']['dir_path']}/", 'public://', $entity->uri[0]);
  }
}
