<?php
/**
 * @file content/field/nodereference.inc
 */

/**
 *
 */
class HCNCFNodeRef extends HCNodeContentFieldBase {
  protected function mapping() {
    parent::mapping();
    $this->addFieldMapping("{$this->sfname}_target_id", "{$this->sfname}_nid");
  }

  public function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = parent::query();
    $select->condition("{$this->sfname}_nid", '', '<>');
    db_set_active();
    return $select;
  }
}
