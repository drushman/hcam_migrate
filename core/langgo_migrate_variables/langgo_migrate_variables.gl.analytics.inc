<?php

/**
 * Migrate Google Analytics settings.
 */
class LanggoMigrateGoogleAnalyticsSettingsMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate Google Analytics Settings';

    // Mapping
    $map_dest = $map_source = MigrateDestinationTable::getKeySchema('variable');
    $this->map = new MigrateSQLMap($this->machineName, $map_source, $map_dest);

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('variable', 'variable');
    $select->fields('variable');
    $select->condition('name', 'googleanalytics%', 'LIKE');
    $this->source = new MigrateSourceSQL($select);

    // Destination
    db_set_active();
    $this->destination = new HCUserSettingsDestinationTable('variable');

    // Field Mapping
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('value', 'value');
  }

}
