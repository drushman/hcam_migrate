<?php
/**
 * @file field/text.inc
 *
 * Migrate node.field.text from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.text
 */
class HCNodeFieldTextMigration extends LanggoNodeFieldMigrateBase {
  protected $field_type = 'text';

  public function prepare($field, $row) {
    parent::prepare($field, $row);

    // Field Data
    $field->data['foreign keys']['format'] = array(
      'table' => 'filter_format',
      'columns' => array('format' => 'format'),
    );

    $field->data['indexes']['format'][] = 'format';

    $field->data['storage']['details']['sql'] = array(
      'FIELD_LOAD_CURRENT' => array(
        "field_data_{$field->field_name}" => array(
          'value' => "{$field->field_name}_value",
          'format' => "{$field->field_name}_format",
        )
      ),
      'FIELD_LOAD_REVISION' => array(
        "field_revision_{$field->field_name}" => array(
          'value' => "{$field->field_name}_value",
          'format' => "{$field->field_name}_format",
        )
      ),
    );

    if (!empty($row->global_settings['allowed_values'])) {
      $field->data['settings']['allowed_values'] = list_allowed_values_string($row->global_settings['allowed_values']);
    }

    $this->createField($field);
  }
}
