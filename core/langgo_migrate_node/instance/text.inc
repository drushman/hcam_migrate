<?php
/**
 * @file langgo_migrate_node/instance/text.inc
 *
 * Migrate node.field.instance.text from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.instance.text.
 */
class HCNodeFieldInstanceTextMigration extends LanggoNodeFieldInstanceMigrationBase {
  protected $source_migration = 'HCNodeFieldText';
  protected $widget_module    = 'text';
  protected $widget_type      = array('text_textfield', 'text_textarea');

  public function prepare($entity, $row) {
    parent::prepare($entity, $row);

    $entity->data['settings']['text_processing'] = $row->global_settings['text_processing'];
    $entity->data['default_value'] = $row->widget_settings['default_value'];

    // Option select
    if (!empty($row->global_settings['allowed_values'])) {
      $entity->data['widget']['type'] = 'options_select';
      $entity->data['widget']['module'] = 'options';
    }
    else {
      $entity->data['widget']['type'] = $row->widget_type;
      $entity->data['widget']['module'] = 'text';
    }
  }
}
