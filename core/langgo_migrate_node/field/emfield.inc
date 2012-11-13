<?php
/**
 * @file field/text.inc
 *
 * Migrate node.field.text from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.text
 */
class HCNodeFieldEmfieldMigration extends LanggoNodeFieldMigrateBase {
  protected $field_type = 'emvideo';

  public function __construct() {
    parent::__construct();
  }

  public function prepare($field, $row) {
    parent::prepare($field, $row);

    $field->type = 'file';
    $field->module = 'file';

    $sql = array(
      'fid' => "{$field->field_name}_fid",
      'title' => "{$field->field_name}_title",
      'data' => "{$field->field_name}_data",
    );

    $field->data['storage']['details']['sql']['FIELD_LOAD_CURRENT']["field_data_{$field->field_name}"] = $sql;
    $field->data['storage']['details']['sql']['FIELD_LOAD_CURRENT']["field_revision_{$field->field_name}"] = $sql;

    $field->data['foreign keys']['file_managed']['table'] = 'file_managed';
    $field->data['foreign keys']['file_managed']['columns']['fid'] = 'fid';
    $field->data['indexes']['fid'][0] = 'fid';

    $this->createField($field);
  }
}
