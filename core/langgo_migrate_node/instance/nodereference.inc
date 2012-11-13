<?php
/**
 * @file instance/userreference.inc
 *
 * Migrate node.field.instance.userreference from Drupal 6 to 7.
 */

/**
 * Class to migrate node reference fields.
 */
class HCNodeFieldInstanceNodeReferenceMigration extends LanggoNodeFieldInstanceReferenceMigrateBase {
  protected $source_migration = 'HCNodeFieldNodeReference';
  protected $widget_module    = 'nodereference';
  protected $widget_type      = array('nodereference_autocomplete', 'nodereference_select');
  protected $entity_type      = 'node';

  public function prepare($entity, $row) {
    parent::prepare($entity, $row);
    $entity->data['widget']['settings']['path'] = '';
    $entity->data['default_value'][0]['target_id'] = $row->widget_settings['default_value'][0]['nid'];
  }
}
