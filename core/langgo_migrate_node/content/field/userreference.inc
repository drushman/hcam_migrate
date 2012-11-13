<?php
/**
 * @file content/field/userreference.inc
 */

/**
 *
 */
class HCNCFUserRef extends HCNodeContentFieldBase {
  protected function mapping() {
    parent::mapping();
    $this->addFieldMapping("{$this->sfname}_target_id", "{$this->sfname}_uid");
  }

  public function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = parent::query();
    $select->condition("{$this->sfname}_uid", '', '<>');
    db_set_active();
    return $select;
  }
}
