<?php
/**
 * @file base/field.instance.reference.inc
 */

/**
 * Base class to help migrate reference field instances.
 */
abstract class LanggoNodeFieldInstanceReferenceMigrateBase extends LanggoNodeFieldInstanceMigrationBase {
  protected $entity_type = 'user';

  public function prepare($entity, $row) {
    parent::prepare($entity, $row);

    $entity->data['settings']['target_type'] = $this->entity_type;

    // Matching rule
    switch ($row->widget_settings['autocomplete_match']) {
      case 'starts_with':
        $match_operator = 'STARTS_WITH';
        break;
      default:
        $match_operator = 'CONTAINS';
    }

    $entity->data['widget']['settings'] = array(
      'match_operator' => $match_operator,
      'size' => $row->widget_settings['size'],
    );

    // Widget module/type
    if (strpos($row->widget_type, '_autocomplete') === 0) {
      $entity->data['widget']['module'] = 'entityreference';
      $entity->data['widget']['type'] = 'entityreference_autocomplete';
    }
    else {
      $entity->data['widget']['module'] = 'option';
      $entity->data['widget']['type'] = 'options_select';
    }
  }
}
