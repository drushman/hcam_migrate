<?php
/**
 * @file field/filefield.inc
 */

/**
 * Class to migrate node.fied.fieldfield
 *
 * @NOTE: On field instance migration, update field:
 *  - `storage` settings to add alt, title, width, height
 *  - Update module/type to image/image
 */
class HCNodeFieldFileMigration extends LanggoNodeFieldMigrateBase {
  protected $field_type = 'filefield';

  public function prepare($field, $row) {
    parent::prepare($field, $row);

    $field->type = 'file';
    $field->module = 'file';

    $field->data['settings']['display_field'] = $row->global_settings['list_field'];
    $field->data['settings']['display_default'] = $row->global_settings['list_default'];
    $field->data['settings']['uri_scheme'] = 'public';

    $field->data['foreign keys']['fid'] = array(
      'table' => 'file_managed',
      'columns' => array('fid' => 'fid'),
    );
    $field->data['indexes']['fid'][0] = 'fid';

    $sql = array(
      'fid' => "{$field->field_name}_fid",
      'display' => "{$field->field_name}_display",
      'description' => "{$field->field_name}_description",
    );

    $field->data['storage']['details']['sql'] = array(
      'FIELD_LOAD_CURRENT' => array(
        "field_data_{$field->field_name}" => $sql,
      ),
      'FIELD_LOAD_REVISION' => array(
        "field_revision_{$field->field_name}" => $sql,
      ),
    );

    $this->createField($field);
  }
}
