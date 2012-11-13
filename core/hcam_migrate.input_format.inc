<?php
/**
 * @file langgo_migrate.input_format.inc
 *
 * Migrate Input formats.
 */

/**
 * Migrate input format.
 */
class HCInputFormatMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = t('Migrate Input Format');
    $this->dependencies[] = 'HCUserRoles';

    // Key Mapping
    $table_name = 'filter_format';
    $this->map = new MigrateSQLMap($this->machineName,
      array('name' => array('type' => 'varchar', 'length' => 255)),
      MigrateDestinationTable::getKeySchema($table_name)
    );

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('filter_formats', 'f');
    $select->fields('f');
    $this->source = new MigrateSourceSQL($select);

    // Destination
    $this->destination = new MigrateDestinationTable($table_name);

    // Field Mappings
    db_set_active();
    $this->addFieldMapping('format', 'name');
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('cache', 'cache');
    $this->addFieldMapping('status')->defaultValue(1);
  }

  public function prepare(&$entity, $row) {
    $name = strtolower($row->name);
    $name = transliteration_get($name);
    $name = preg_replace('/\s+/', '_', $name);
    $entity->format = $name;

    // Grant permission to role
    $roles = trim($row->roles, ',');
    foreach (explode(',', $roles) as $old_role_id) {
      $old_role_id = trim($old_role_id, '');
      if (!$old_role_id) continue;

      if ($role_id = Migration::getInstance('HCUserRoles')->getMap()->lookupDestinationID(array($old_role_id))) {
        $role_id = reset($role_id);
        $merge = db_merge('role_permission');
        $merge->key(array('rid' => $role_id, 'permission' => "use text format {$name}"));
        $merge->fields(array('module' => 'filter'));
        $merge->execute();
      }
    }
  }

  public function prepareRow(stdClass $row) {
    $format = strtolower($row->name);
    $format = transliteration_get($format);
    $format = preg_replace('/\s+/', '_', $format);
    db_delete('filter_format')->condition('format', $format)->execute();
    db_delete('filter')->condition('format', $format)->execute();
  }
}

/**
 * Migrate filters.
 */
class HCFiltersMigration extends LanggoMigrateBase {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate Input Format');
    $this->dependencies = array('HCInputFormat');

    // Key Mapping
    $keys = MigrateDestinationTable::getKeySchema('filter');
    $this->map = new MigrateSQLMap(
      $this->machineName,
      array('fid' => array('type' => 'int')),
      $keys
    );

    // Source
    $this->query();
    $this->destination = new MigrateDestinationTable('filter');
    $this->mapping();
  }

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('filters');
    $query->leftjoin('filter_formats', 'filter_formats', 'filters.format = filter_formats.format');
    $query->fields('filters', array('fid', 'module', 'delta', 'weight', 'format'));
    $query->addExpression("REPLACE(LOWER(filter_formats.name), ' ', '_')", 'name');
    $this->source = new MigrateSourceSQL($query);
    db_set_active();
  }

  protected function mapping() {
    $this->addFieldMapping('module', 'module');
    $this->addFieldMapping('weight', 'weight');
    $this->addFieldMapping('name', 'delta');
    $this->addFieldMapping('format', 'name');
    $this->addFieldMapping('status', NULL)->defaultValue(1);
    $this->addFieldMapping('settings', NULL)->defaultValue(array());
    $this->addUnmigratedSources(array('fid', 'format'));
  }

  /**
   * filter_update_7000: Upgrade the {filter_formats} table and rename it to {filter_format}.
   */
  public function prepare($filter, $row) {
    $filter->name = $this->getDelta($filter->module, $filter->name);
    $settings = $this->fu7003($filter, $row);
  }

  /**
   * filter_update_7001: Break out "escape HTML filter" option to its own filter.
   */
  private function fu7001(&$entity, $row) {
    if ($entity->module !== 'filter') return;
    if ($entity->name !== 'filter_html_escape') return;

    if ($filter_html = $this->variableGet("filter_html_{$row->format}")) {
      // Deprecated constants FILTER_HTML_STRIP = 1 and FILTER_HTML_ESCAPE = 2.
      if ($filter_html) {
        db_merge('filter')
          ->key(array('format' => $entity->format, 'name' => $entity->name))
          ->fields(array(
            'format' => $entity->format,
            'name' => $entity->name,
            'module' => 'filter',
            'weight' => 0,
            'status' => 1,
            'settings' => array()))
          ->execute();
      }
    }
  }

  /**
   * filter_update_7003: filter delta => filter name, settings to a row.
   */
  private function fu7003(&$entity, $row) {
    if ($entity->module === 'filter') {
      if ($entity->name === 'filter_html') {
        if ($settings = $this->fu7003FilterHtml($entity, $row)) {
          return $settings;
        }
      }
      elseif ($entity->name === 'filter_url') {
        if ($setting = $this->fu7003FilterUrl($entity, $row)) {
          $settings['filter_url_length'] = $setting;
          return $settings;
        }
      }
    }
  }

  private function fu7003FilterHtml(&$entity, $row) {
    $settings = array();

    if ($v = $this->variableGet("allowed_html_{$row->format}")) {
      $settings['allowed_html'] = $v;
    }

    if ($v = $this->variableGet("filter_html_help_{$row->format}")) {
      $settings['filter_html_help'] = $v;
    }

    if ($v = $this->variableGet("filter_html_nofollow_{$row->format}")) {
      $settings['filter_html_nofollow'] = $v;
    }

    return $settings;
  }

  private function fu7003FilterUrl(&$entity, $row) {
    if ($v = $this->variableGet("filter_url_length_{$row->format}")) {
      return $v;
    }
  }

  /**
   * @TODO: Wrong, need pass module value.
   */
  protected function getDelta($module, $delta) {
    $renamed_deltas['php'] = array('0' => 'php_code');
    $renamed_deltas['filter'] = array(
      '0' => 'filter_html',
      '1' => 'filter_autop',
      '2' => 'filter_url',
      '3' => 'filter_htmlcorrector',
      '4' => 'filter_html_escape',
    );
    return $renamed_deltas['filter'][$delta];
  }
}
