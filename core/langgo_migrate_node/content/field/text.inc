<?php
/**
 * @file content/field/text.inc
 */

/**
 *
 */
class HCNCFText extends HCNodeContentFieldBase {
  protected function mapping() {
    parent::mapping();

    foreach (array_keys($this->sfi->db_columns) as $k) {
      $this->addFieldMapping("{$this->sfname}_{$k}", "{$this->sfname}_{$k}");
    }

    if (!isset($this->sfi->db_columns['format'])) {
      $this->addFieldMapping("{$this->sfname}_format")->defaultValue('');
    }
  }

  public function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = parent::query();
    $select->condition("{$this->sfname}_value", '', '<>');
    db_set_active();
    return $select;
  }
}
