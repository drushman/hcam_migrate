<?php
/**
 * @file instance/userreference.inc
 *
 * Migrate node.field.instance.userreference from Drupal 6 to 7.
 */

/**
 * Class to migrate user reference fields.
 */
class HCNodeFieldInstanceUserReferenceMigration extends LanggoNodeFieldInstanceReferenceMigrateBase {
  protected $source_migration = 'HCNodeFieldUserReference';
  protected $widget_module    = 'userreference';
  protected $widget_type      = array('userreference_autocomplete', 'userreference_select');
  protected $entity_type      = 'user';

  public function prepare($entity, $row) {
    parent::prepare($entity, $row);
    $entity->data['default_value'][0]['target_id'] = $row->widget_settings['default_value'][0]['uid'];
  }
}
