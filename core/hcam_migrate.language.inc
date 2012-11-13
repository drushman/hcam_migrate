<?php
/**
 * @file langgo_migrate.language.inc
 */

/**
 * Migrate Languages.
 */
class HCLanguageMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Langgo'));
    $this->description = 'Migrate Languages';

    // Mapping
    $map_dest = $map_source = MigrateDestinationTable::getKeySchema('languages');
    $this->map = new MigrateSQLMap($this->machineName, $map_source, $map_dest);

    // Source
    db_set_active(LANGGO_MIGRATE_MAIN_DB_KEY);
    $select = db_select('languages', 'languages');
    $select->fields('languages');
    $this->source = new MigrateSourceSQL($select);

    // Destination
    db_set_active();
    $this->destination = new HCLanguageDestinationTable('languages');

    // Field Mapping
    $this->addFieldMapping('language', 'language');
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('native', 'native');
    $this->addFieldMapping('direction', 'direction');
    $this->addFieldMapping('enabled', 'enabled');
    $this->addFieldMapping('plurals', 'plurals');
    $this->addFieldMapping('formula', 'formula');
    $this->addFieldMapping('domain', 'domain');
    $this->addFieldMapping('prefix', 'prefix');
    $this->addFieldMapping('weight', 'weight');
    $this->addFieldMapping('javascript', 'javascript');
  }
}

/**
 * Wrapper for destination.
 */
class HCLanguageDestinationTable extends MigrateDestinationTable{
  public function import(stdClass $entity, stdClass $row) {
    $delete = db_delete('languages');
    $delete->condition('language', $row->language);
    $delete->execute();
    return parent::import($entity, $row);
  }
}
