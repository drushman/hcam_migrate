<?php
/**
 * @file langgo_migrate.user.inc
 *
 * Classes to migrate users: roles, pictures, user data, settings.
 */

/**
 * Migrate user settings.
 */
class HCUserSettingsMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate User Settings';

    // Mapping
    $map_dest = $map_source = MigrateDestinationTable::getKeySchema('variable');
    $this->map = new MigrateSQLMap($this->machineName, $map_source, $map_dest);

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('variable', 'variable');
    $select->fields('variable');
    $select->condition('name', 'user%', 'LIKE');
    $this->source = new MigrateSourceSQL($select);

    // Destination
    db_set_active();
    $this->destination = new HCUserSettingsDestinationTable('variable');

    // Field Mapping
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('value', 'value');
  }

  public function __destruct() {
    module_load_include('install', 'user');
    user_update_7017();
    cache_clear_all();
  }
}

/**
 * Wrapper for destination.
 */
class HCUserSettingsDestinationTable extends MigrateDestinationTable {
  public function import(stdClass $entity, stdClass $row) {
    variable_del($entity->name);
    return parent::import($entity, $row);
  }
}

/**
 * Migrate file picture of users
 */
class HCUserPicturesMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = t('Migrate Pitures of Users');

    $this->map = new MigrateSQLMap($this->machineName,
      MigrateDestinationUser::getKeySchema(),
      MigrateDestinationFile::getKeySchema()
    );

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('users', 'u')
             ->fields('u', array('uid', 'picture', 'name'))
             ->condition('picture', '', '<>');
    $this->source = new MigrateSourceSQL($query);
    db_set_active();

    // Destination
    $this->destination = new MigrateDestinationFile();

    // Field Mappings
    // In the simplest case, just map the incoming URL to 'value'.
    $this->addFieldMapping('value', 'picture');

    $this->addUnmigratedDestinations(array('fid', 'uid', 'timestamp',
      'destination_dir', 'destination_file', 'source_dir', 'preserve_files',
      'file_replace'));
  }
}

/**
 * Migrate role
 *
 * @TODO: Check this http://drupalcode.org/sandbox/mikeryan/1234554.git/blob/refs/heads/7.x-2.x:/role.inc
 */
class HCUserRolesMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = t('Migrate Roles');
    $this->dependencies[] = 'HCUserPictures';

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'rid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        )
      ),
      MigrateDestinationRole::getKeySchema()
    );
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('role', 'r')
      ->fields('r', array('rid', 'name'))
      ->orderBy('r.rid', 'ASC');
    $this->source = new MigrateSourceSQL($query);

    $this->destination = new MigrateDestinationRole();
    db_set_active();
    $this->addFieldMapping('name', 'name');
    $this->addUnmigratedDestinations(array('weight'));
  }

  public function prepareRow(stdClass $row) {
    $query = db_delete('role')->condition('name', $row->name)->execute();
  }
}

/**
 * Migrate Users
 *
 * @TODO: Check this http://drupalcode.org/sandbox/mikeryan/1234554.git/blob/refs/heads/7.x-2.x:/user.inc
 */
class HCUserMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = t('Migrate users from drupal 6 to 7');
    $this->dependencies[] = 'HCUserRoles';
    $this->dependencies[] = 'HCUserPictures';
//    $this->dependencies[] = 'HCLanguage';
//    $this->dependencies[] = 'HCInputFormat';

    // Key Mapping
    $keys = MigrateDestinationUser::getKeySchema();
    $source_keys = $keys;
    $source_keys['uid']['alias'] = 'u';
    $this->map = new MigrateSQLMap($this->machineName, $source_keys, $keys);

    // Source
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('users', 'u');
    $query->fields('u');
    $query->leftJoin('users_roles', 'ur', 'ur.uid = u.uid');
    $query->groupBy('u.uid');
    $query->addExpression('GROUP_CONCAT(ur.rid)', 'roles');
    $query->condition('u.uid', 1, '>=');

    $this->source = new MigrateSourceSQL($query);

    // Destination
    db_set_active();
    $this->destination = new MigrateDestinationUser(array('md5_passwords' => TRUE));

    // Field Mapping
    $this->addSimpleMappings(array('name', 'status', 'mail'));
    $this->addFieldMapping('created', 'created')->description('See prepare method');
    $this->addFieldMapping('access', 'access')->description('See prepare method');
    $this->addFieldMapping('login', 'login')->description('See prepare method');
    $this->addFieldMapping('pass', 'pass');
    $this->addFieldMapping('roles', 'roles')->separator(',')->sourceMigration('HCUserRoles');
    $this->addFieldMapping('signature', 'signature');
    $this->addFieldMapping('signature_format','signature_format');
    $this->addFieldMapping('init', 'init');
    # $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('theme', 'theme');
    # $this->addFieldMapping('data', 'data');
    # $this->addFieldMapping('timezone', 'timezone');
    $this->addFieldMapping('language', 'language');
    $this->addFieldMapping('picture', 'uid')->sourceMigration('HCUserPictures');

    // @TODO: Don't ignore timezone
    $this->addUnmigratedDestinations(array('timezone','data'));
  }

  public function prepareRow(stdClass $row) {
    if ($user = user_load_by_name($row->name) && $row->uid != 1) {
      user_delete($row->uid);
    }
  }

  public function prepare(stdClass &$user,stdClass $row) {
    $check = $row->uid == 1;
    $check &= $row->name == $user->name;
    if($check){
      $user->name = 'langgo_migrate_' . $user->name;
      $user->roles[3] = 3;
    }
  }
}
