<?php
/**
 * @file langgo_migrate_main.block.inc
 *
 * Classes to migrate Drupal static blocks.
 */

/**
 * Migrate table block
 */
class HCBlocksMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate table blocks(drupal6) to block (drupal7)';
    $table_name = 'block';
    $this->map = new MigrateSQLMap($this->machineName,
      array('bid' => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE)),
      MigrateDestinationTable::getKeySchema($table_name)
    );
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('blocks', 'b')
             ->fields('b', array('bid', 'module', 'delta', 'theme', 'status',
                      'weight', 'region', 'custom', 'visibility', 'pages', 'title', 'cache'));
    $this->source = new MigrateSourceSQL($query);
    db_set_active();
    $this->destination = new MigrateDestinationTable($table_name);

    // Mapped fields
    $this->addFieldMapping('module', 'module');
    $this->addFieldMapping('delta', 'delta');
    $this->addFieldMapping('theme', 'theme');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('weight', 'weight');
    $this->addFieldMapping('region', 'region');
    $this->addFieldMapping('custom', 'custom');
    $this->addFieldMapping('visibility', 'visibility');
    $this->addFieldMapping('pages', 'pages');
    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('cache', 'cache');
  }
}

/**
 * Migrate table block
 */
class HCBlocksRolesMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate table blocks_roles(drupal6) to block (drupal7)';
    $this->dependencies[] = 'HCUserRoles';
    $table_name = 'block_role';
    $keys = MigrateDestinationTable::getKeySchema($table_name);
    $this->map = new MigrateSQLMap($this->machineName, $keys, $keys);

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('blocks_roles');
    $query->fields('blocks_roles');
    $this->source = new MigrateSourceSQL($query);
    db_set_active();

    // Destination
    $this->destination = new MigrateDestinationTable($table_name);

    // Mapped fields
    $this->addFieldMapping('module', 'module');
    $this->addFieldMapping('delta', 'delta');
    $this->addFieldMapping('rid', 'rid')->sourceMigration('HCUserRoles');
  }

  public function prepare(&$node, $row) {
    $check = !isset($node->rid);
    $check = $check &&  isset($row->rid);
    if ($check) {
      $node->rid = $row->rid;
    }
  }
}

/**
 * Migrate table block custom
 */
class HCBoxesMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate table boxes(drupal6) to block_custom (drupal7)';
    $table_name = 'block_custom';
    $this->dependencies = array('HCBlocks', 'HCInputFormat');
    $this->map = new MigrateSQLMap($this->machineName,
      array('bid' => array(
              'type' => 'int',
              'unsigned' => TRUE,
              //'not null' => TRUE,
              'alias' => 'b',
             )
           ),
      MigrateDestinationTable::getKeySchema($table_name)
    );
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('boxes', 'b')
            ->fields('b', array('bid', 'body', 'info'))
            ->fields('f', array('name'));
    $query->leftjoin('filter_formats', 'f', 'f.format = b.format');

    $this->source = new MigrateSourceSQL($query);
    db_set_active();
    $this->destination = new MigrateDestinationTable($table_name);


    // Mapped fields
    $this->addFieldMapping('body', 'body');
    $this->addFieldMapping('info', 'info');
    $this->addFieldMapping('format', 'name')
         ->sourceMigration('HCInputFormat');
    $this->addFieldMapping('bid', 'bid')
         ->sourceMigration('HCBlocks');
  }
}
