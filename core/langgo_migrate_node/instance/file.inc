<?php
/**
 * @file langgo_migrate_node/instance/text.inc
 *
 * Migrate node.field.instance.text from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.instance.text.
 */
class HCNodeFieldInstanceFileMigration extends LanggoNodeFieldInstanceMigrationBase {
  protected $source_migration = 'HCNodeFieldFile';
  protected $widget_module    = 'filefield';
  protected $widget_type      = 'filefield_widget';

//  public function __construct() {
//    parent::__construct();
//    LanggoNodeHelper::d6Info('field_plesson_video');
//    LanggoNodeHelper::d7Info('field_my_file');
//  }

  public function prepare($entity, $row) {
    parent::prepare($entity, $row);

    $entity->data['widget']['module'] = 'file';
    $entity->data['widget']['type'] = 'file_generic';
    $entity->data['widget']['settings']['progress_indicator'] = $row->widget_settings['progress_indicator'];
    $entity->data['settings']['file_directory'] = $row->widget_settings['file_path'];
    $entity->data['settings']['file_extensions'] = $row->widget_settings['file_extensions'];
    $entity->data['settings']['max_filesize'] = $row->widget_settings['max_filesize_per_file'];
    $entity->data['settings']['description_field'] = $row->widget_settings['description_field'];
  }
}
