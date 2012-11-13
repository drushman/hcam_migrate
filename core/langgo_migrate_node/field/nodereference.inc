<?php
/**
 * @file field/nodereference.inc
 *
 * Migrate node.field.nodereference from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.nodereference
 */
class HCNodeFieldNodeReferenceMigration extends LanggoNodeFieldReferenceMigrateBase {
  protected $field_type = 'nodereference';
  protected $entity_type = 'node';
  protected $sort = 'property|title|body:value|ASC';

  public function prepare($field, $row) {
    parent::prepare($field, $row);
    $field->data['settings']['handler_settings']['target_bundles'] = array_filter($row->global_settings['referenceable_types']);
    $this->createField($field);
  }
}
