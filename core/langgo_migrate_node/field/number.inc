<?php
/**
 * @file field/number.inc
 *
 * Migrate node.field.number from Drupal 6 to 7.
 */

/**
 * Class to migrate node.field.number
 *
 * @TODO: Just test on number_integer.
 */
class HCNodeFieldNumberMigration extends LanggoNodeFieldMigrateBase {
  protected $field_type = 'number_integer';

  public function prepare($field, $row) {
    parent::prepare($field, $row);

    $field->data['foreign keys'] = array();
    $field->data['indexes'] = array();

    $field->data['storage']['details']['sql'] = array(
      'FIELD_LOAD_CURRENT' => array(
        "field_data_{$field->field_name}" => array(
          'value' => "{$field->field_name}_value",
        )
      ),
      'FIELD_LOAD_REVISION' => array(
        "field_revision_{$field->field_name}" => array(
          'value' => "{$field->field_name}_value",
        )
      ),
    );

    if (!empty($row->global_settings['allowed_values'])) {
      $field->data['settings']['allowed_values'] = list_allowed_values_string($row->global_settings['allowed_values']);
    }

    $this->createField($field);
  }
}
