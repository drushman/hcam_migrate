<?php
/**
 * @file field/userreference.inc
 *
 * Migrate node.field.userreference from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.userreference
 */
class HCNodeFieldUserReferenceMigration extends LanggoNodeFieldReferenceMigrateBase {
  protected $field_type = 'userreference';
  protected $entity_type = 'user';
  protected $sort = 'property|name|property|ASC';

  public function prepare($field, $row) {
    parent::prepare($field, $row);
    $this->createField($field);
  }
}
