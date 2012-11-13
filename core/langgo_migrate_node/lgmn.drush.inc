<?php
/**
 * @file langgo_migrate.drush.inc
 *
 * Provide Drush commands.
 */

/**
 * Implements hook_drush_command().
 */
function HCn_drush_command() {
  $items['HCn-wrapper']['description'] = 'Wrapper to call content migrations.';
  $items['HCn-content']['description'] = 'Wrapper to call content field migrations.';
  return $items;
}

/**
 * Wrapper to execute content migrations.
 */
function drush_HCn_wrapper() {
  drush_print("Importing HCNodeTypes");
  drush_do_command_redispatch("migrate-import HCNodeTypes");

  foreach (LanggoNodeHelper::getNodeBasicContentMigrations() as $migration) {
    drush_print("Importing {$migration}");
    drush_do_command_redispatch("migrate-import {$migration}");
  }

  foreach (LanggoNodeHelper::getNodeFieldMigrations() as $migration) {
    drush_print("Importing {$migration}");
    drush_do_command_redispatch("migrate-import {$migration}");
  }

  foreach (LanggoNodeHelper::getNodeFieldInstanceMigrations() as $migration) {
    drush_print("Importing {$migration}");
    drush_do_command_redispatch("migrate-import {$migration}");
  }

  drush_do_command_redispatch("HCn-content-fields");
}

function drush_HCn_content() {
  foreach (HCn_find_source_node_field_instances() as $sftype => $sfnames) {
    $class_name = '';

    switch ($sftype) {
      case 'filefield':
        $class_name = 'HCNCFFile';
        break;
      case 'text':
        $class_name = 'HCNCFText';
        break;
      case 'number_integer':
        $class_name = 'HCNCFNumberInteger';
        break;
      case 'userreference':
        $class_name = 'HCNCFUserRef';
        break;
      case 'nodereference':
        $class_name = 'HCNCFNodeRef';
        break;
      case 'emvideo':
        $class_name = 'HCNCFEmfield';
        break;
      case 'datestamp':
        $class_name = 'HCNCFDate';
        break;
    }

    if (!empty($class_name)) {
      foreach ($sfnames as $row) {
        $machine_name = "HCnc{$sftype}_" . substr(md5("{$class_name}_{$row['sbundle']}_{$row['sfname']}"), 0, 10);
        $arguments = array('sbundle' => $row['sbundle'], 'sfname' => $row['sfname'], 'machine_name' => $machine_name);
        Migration::registerMigration($class_name, $machine_name, $arguments);

        drush_print("Importing {$machine_name}");
        drush_do_command_redispatch("migrate-import {$machine_name}");
      }
    }
  }
}

function HCn_find_source_node_field_instances() {
  db_set_active(HCAM_MIGRATE_DB_KEY);
  $select = db_select('content_node_field_instance', 'fi');
  $select->innerJoin('content_node_field', 'f', 'fi.field_name = f.field_name');
  $select->fields('fi', array('field_name', 'type_name'));
  $select->fields('f', array('type'));
  $select->condition('f.type', array('computed', 'datestamp'), 'NOT IN');
  $select->orderBy('f.type');
  foreach ($select->execute()->fetchAll() as $row) {
    $field_names[$row->type][] = array(
      'sfname'  => $row->field_name,
      'sbundle' => $row->type_name
    );
  }
  db_set_active();
  return $field_names;
}
