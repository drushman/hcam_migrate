<?php
/**
 * @file langgo_migrate_node/instance/number.inc
 *
 * Migrate node.field.instance.number from Drupal 6 to 7.
 */

class HCNodeFieldInstanceNumberMigration extends LanggoNodeFieldInstanceMigrationBase {
  protected $source_migration = 'HCNodeFieldNumber';
  protected $widget_module    = 'number';
  protected $widget_type      = 'number';

  public function prepare($entity, $row) {
    parent::prepare($entity, $row);

    $entity->data['default_value'][0]['value'] = $row->widget_settings['default_value'][0]['value'];
    $entity->data['settings'] += array(
      'min' => $row->widget_settings['min'],
      'max' => $row->widget_settings['max'],
      'prefix' => $row->widget_settings['prefix'],
      'suffix' => $row->widget_settings['suffix'],
    );

    // Option select
    if (!empty($row->global_settings['allowed_values'])) {
      $entity->data['widget']['type'] = 'options_select';
      $entity->data['widget']['module'] = 'options';
    }
    else {
      $entity->data['widget']['type'] = $row->widget_type;
      $entity->data['widget']['module'] = 'number';
    }
  }
}
