<?php
/**
 * @file langgo_migrate_node.base.inc
 *
 */

class LanggoNodeHelper {
  public static function getExcludedBundleModules () {
    return array('uc_product', 'poll');
  }

  public static function d6Info($field_name) {
    dsm('========= '. __FUNCTION__ .' =========');
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('content_node_field_instance', 'fi');
    $select->innerJoin('content_node_field', 'f', 'fi.field_name = f.field_name');
    $select->condition('fi.field_name', $field_name);
    $select->fields('fi', array('field_name', 'widget_settings', 'display_settings'));
    $select->fields('f', array('global_settings', 'db_columns'));
    foreach ($select->execute()->fetchAll() as $row) {
      dsm($row->field_name);
      dsm(array(
        'widget_settings'  => unserialize($row->widget_settings),
        'display_settings' => unserialize($row->display_settings),
        'global_settings'  => unserialize($row->global_settings),
        'db_columns'       => unserialize($row->db_columns),
      ));
    }
    db_set_active();
  }

  public static function d7Info($field_name) {
    dsm('========= '. __FUNCTION__ .' =========');
    $select = db_select('field_config_instance', 'fi');
    $select->innerJoin('field_config', 'f', 'fi.field_id = f.id');
    $select->fields('fi', array('entity_type', 'bundle', 'field_name'));
    $select->addField('f', 'data', 'field_data');
    $select->addField('fi', 'data', 'instance_data');
    $select->condition('f.field_name', $field_name);
    foreach ($select->execute()->fetchAll() as $row) {
      dsm("{$row->entity_type}:{$row->bundle}:{$row->field_name}");
      dsm(array(
        'field_data' => unserialize($row->field_data),
        'instance_data' => unserialize($row->instance_data),
      ));
    }
  }

  private static function getMigrations($pattern) {
    $select = db_select('registry');
    $select->condition('registry.module', 'langgo_migrate_node');
    $select->condition('registry.type', 'class');
    $select->condition('registry.name', $pattern, 'LIKE');
    $select->fields('registry', array('name'));
    foreach ($select->execute()->fetchAll() as $row) {
      if (substr($row->name, -9) === 'Migration') {
        $row->name = substr($row->name, 0, strlen($row->name) - 9);
      }
      $classes[] = $row->name;
    }
    return $classes ? $classes : array();
  }

  public static function getNodeBasicContentMigrations() {
    return self::getMigrations('LanggoNodeBasicContent%');
  }

  public static function getNodeFieldMigrations() {
    foreach (self::getMigrations('HCNodeField%') as $class) {
      if (strpos($class, 'HCNodeFieldInstance') !== 0) {
        $classes[] = $class;
      }
    }
    return $classes ? $classes : array();
  }

  public static function getNodeFieldInstanceMigrations() {
    return self::getMigrations('HCNodeFieldInstance%');
  }
}

/**
 * Base class to migrate node content.
 *
 * @TODO: Auto add body field to bundle if not exists.
 */
abstract class LanggoNodeMigrateBase extends LanggoMigrateBase {
  var $bundle;
  var $bundleInfo;
  var $exclude_field_modules = array('rolereference', '');

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = $this->countQuery();
    $query->leftJoin('url_alias', 'url_alias', "CONCAT('node/', node.nid) = url_alias.src");
    $query->leftJoin('filter_formats', 'filter_formats', 'node_revisions.format = filter_formats.format');
    $query->fields('node', array('nid', 'language', 'uid', 'status', 'created', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'));
    $query->fields('node_revisions', array('vid', 'title', 'body', 'teaser', 'format', 'log'));
    $query->addField('node_revisions', 'uid', 'revision_uid');
    $query->addField('node_revisions', 'timestamp', 'changed');
    $query->addField('url_alias', 'dst', 'path');
    $query->addExpression("REPLACE(LOWER(filter_formats.name), ' ', '_')", 'format');
    foreach ($this->getFields() as $field_name => $field) {
      if ($field->instance_counter == 1) {
        $table = "content_type_{$field->type_name}";
      }
      else {
        $table = "content_{$field_name}";
      }
      $query->innerJoin($table, $table, "node_revisions.vid = {$table}.vid");
      foreach ($field->db_columns as $col => $inf) {
        $query->addField($table, "{$field_name}_{$col}", "{$field_name}_{$col}");
      }
    }
    db_set_active();
    return $query;
  }

  protected function mapping() {
    // …

    foreach ($this->getDestination()->fields() as $field_name => $description) {
      if (substr($field_name, -8, 8) === 'language') {
        $this->addFieldMapping($field_name, NULL)->defaultValue(LANGUAGE_NONE);
      }
      elseif (substr($field_name, -10, 10) === 'file_class') {
        $this->addUnmigratedDestinations(array($field_name));
      }
      else {
        $check = array('source_dir', 'file_class', 'destination_dir', 'destination_file', 'preserve_files', 'file_replace');
        foreach ($check as $string) {
          $len = strlen($string);
          if (substr($field_name, -1 * $len, $len) === $string) {
            $this->addUnmigratedDestinations(array($field_name));
          }
        }
      }
    }
  }
}
