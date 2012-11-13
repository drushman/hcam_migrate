<?php
/**
 * @file base/field.reference.inc
 *
 * Base class to help migrate reference fields.
 */

abstract class LanggoNodeFieldReferenceMigrateBase extends LanggoNodeFieldMigrateBase {
  protected $field_type = 'userreference';
  protected $entity_type = 'user';
  protected $sort = 'property|name|property|ASC';

  public function prepare($entity, $row) {
    parent::prepare($entity, $row);

    $sort = explode('|', $this->sort);

    $entity->type = 'entityreference';
    $entity->module = 'entityreference';

    $entity->data['settings']['target_type'] = $this->entity_type;
    $entity->data['settings']['handler'] = 'base';
    $entity->data['settings']['handler_settings'] = array(
      'target_bundles' => array(),
      'behaviors' => array('views-select-list' => array('status' => 0)),
      'sort' => array('type' => $sort[0], 'property' => $sort[1], 'field' => $sort[2], 'direction' => $sort[3]),
    );

    $entity->data['storage']['details']['sql'] = array(
      'FIELD_LOAD_CURRENT' => array("field_data_{$entity->field_name}" => array(
        'target_id' => "{$entity->field_name}_target_id",
      )),
      'FIELD_LOAD_REVISION' => array("field_revision_{$entity->field_name}" => array(
        'target_id' => "{$entity->field_name}_target_id",
      )),
    );

    $entity->data['foreign keys']['node'] = array(
      'table' => 'node',
      'columns' => array('target_id' => 'nid')
    );

    $entity->data['indexes']['target_id'] = array('target_id');
  }
}
