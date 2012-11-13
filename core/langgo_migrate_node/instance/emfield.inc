<?php
/**
 * @file langgo_migrate_node/instance/text.inc
 *
 * Migrate node.field.instance.text from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.instance.text.
 */
class HCNodeFieldInstanceEmfieldMigration extends LanggoNodeFieldInstanceMigrationBase {
  protected $source_migration = 'HCNodeFieldEmfield';
  protected $widget_module    = 'emvideo';
  protected $widget_type      = array('emvideo_textfields');

  public function __construct() {
    parent::__construct();
  }

  public function prepare($instance, $row) {
    parent::prepare($instance, $row);

    $instance->data['widget']['type'] = 'media_generic';
    $instance->data['widget']['module'] = 'media';
    $instance->data['widget']['settings']['allowed_types']['video'] = 'video';
    $instance->data['widget']['settings']['allowed_schemes']['youtube'] = 'youtube';
    $instance->data['widget']['settings']['progress_indicator'] = 'throbber';
    $instance->data['settings']['file_extensions'] = '';

    $this->createInstance($instance);
  }
}
