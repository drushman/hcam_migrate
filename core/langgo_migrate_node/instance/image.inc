<?php
/**
 * @file langgo_migrate_node/instance/text.inc
 *
 * Migrate node.field.instance.text from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.instance.text.
 */
class LanggoNodeFieldInstanceImageMigration extends HCNodeFieldInstanceFileMigration {
  protected $source_migration = 'HCNodeFieldFile';
  protected $widget_module    = 'imagefield';
  protected $widget_type      = 'imagefield_widget';

  public function __construct() {
    parent::__construct();
    $this->dependencies[] = 'HCFile';
  }

  public function prepare($entity, $row) {
    parent::prepare($entity, $row);

    $entity->data['widget']['module'] = 'image_image';
    $entity->data['widget']['type'] = 'image';
    $entity->data['widget']['settings']['preview_image_style'] = 'thumbnail';
    $entity->data['settings']['max_resolution'] = $row->widget_settings['max_resolution'];
    $entity->data['settings']['min_resolution'] = $row->widget_settings['min_resolution'];
    $entity->data['settings']['alt_field'] = $row->widget_settings['alt'];
    $entity->data['settings']['title_field'] = $row->widget_settings['title'];
    if (!empty($row->widget_settings['default_image'])) {
      if ($s_fid = $row->widget_settings['default_image']['fid']) {
        if ($d_fid = Migration::getInstance('HCFile')->getMap()->lookupDestinationID(array($s_fid))) {
          $d_fid = reset($d_fid);
          $entity->data['settings']['default_image'] = $d_fid;
        }
      }
    }
  }

  public function complete($entity, $row) {
    $field_id = $entity->field_id;
    $field_data = db_select('field_config');
    $field_data->condition('id', $field_id);
    $field_data->fields('field_config', array('data'));
    if ($field_data = $field_data->execute()->fetchColumn()) {
      $extra = array(
        'alt' => "{$entity->field_name}_alt",
        'title' => "{$entity->field_name}_title",
        'width' => "{$entity->field_name}_width",
        'height' => "{$entity->field_name}_height",
      );

      $field_data['storage']['details']['sql']['FIELD_LOAD_CURRENT']["field_data_{$entity->field_name}"] += $extra;
      $field_data['storage']['details']['sql']['FIELD_LOAD_REVISION']["field_revision_{$entity->field_name}"] += $extra;

      db_update('field_config')
          ->fields(array(
            'type' => 'image',
            'module' => 'image',
            'data' => $field_data
          ))
          ->condition('id', $field_id)
          ->execute();
    }
  }
}
