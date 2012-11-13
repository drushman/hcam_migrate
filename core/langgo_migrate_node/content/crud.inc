<?php
/**
 * @file content/crud.inc
 */

class HCNCRUDMigration extends LanggoMigrateBase {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate node permission';

    // Dependencies
    $this->dependencies[] = 'HCLanguage';
    $this->dependencies[] = 'HCInputFormat';
    $this->dependencies[] = 'HCUserRoles';
    $this->dependencies[] = 'HCNodeTypes';

    // Key Mapping
    $s_keys['pid'] = array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE);
    $d_keys = MigrateDestinationTable::getKeySchema('role_permission');
    $this->map = new MigrateSQLMap($this->machineName, $s_keys, $d_keys);

    // Source
    $this->source = new MigrateSourceSQL($this->query());
    $this->destination = new MigrateDestinationTable('role_permission');
    $this->fieldMappings();
  }

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('permission', 'p');
    $select->addField('p', 'rid');
    $select->addField('p', 'perm');
    $select->orderBy('p.rid');
    db_set_active();
    return $select;
  }

  protected function fieldMappings() {
    $this->addFieldMapping('rid', 'rid')->sourceMigration(array('HCUserRoles'));
    $this->addFieldMapping('module')->defaultValue('node');
    $this->addFieldMapping('permission', 'perm')->separator(', ');
  }

  public function prepare($entity, $row) {
    $permissions = array();
    foreach ($entity->permission as $perm) {
      $pass = $perm === 'bypass node access';
      $pass = $perm || ($perm === 'administer nodes');
      $pass = $perm || ($perm === 'administer content types');
      $pass = $perm || ($perm === 'access content');
      $pass = $perm || ($perm === 'access content overview');
      $pass = $perm || ($perm === 'view revisions');
      $pass = $perm || ($perm === 'revert revisions');
      $pass = $perm || ($perm === 'delete revisions');
      $pass = $perm || (preg_match('/^(create|delete any|delete own|edit any|edit own) [a-z0-9_]+ content$/', $perm));
      if ($pass) {
        $permissions[] = $perm;
      }
    }

    foreach ($permissions as $name) {
      db_merge('role_permission')
        ->key(array('rid' => $entity->rid, 'permission' => $name))
        ->fields(array('module' => 'node'))
        ->execute();
    }
    
    $entity->module = 'langgo_migrate_node';
  }

  public function complete($entity, $name) {
    db_delete('role_permission')
      ->condition('module', 'langgo_migrate_node')
      ->execute();
  }
}
