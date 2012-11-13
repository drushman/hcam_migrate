<?php
/**
 * @file content/field/filefield.inc
 */

class HCNCFFile extends HCNodeContentFieldBase {
  protected function mapping() {
    parent::mapping();

    $this->addFieldMapping("{$this->sfname}_fid", "{$this->sfname}_fid")
         ->sourceMigration('HCFile');
    $this->addFieldMapping("{$this->sfname}_display", "{$this->sfname}_list");
    $this->addFieldMapping("{$this->sfname}_description", "{$this->sfname}_data");
  }

  public function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = parent::query();
    if (($this->sfi->instance_counter > 1) || $this->sfi->multiple <> 0) {
      $select->addField($this->stable, 'delta');
    }
    $select->condition("{$this->sfname}_fid", '', '<>');
    db_set_active();
    return $select;
  }

  public function prepare($entity, stdClass $row) {
    if ($data = unserialize($entity->{"{$this->sfname}_description"})) {
      if (is_array($data) && !empty($data['title'])) {
        $data = $data['title'];
      }
    }
    $entity->{"{$this->sfname}_description"} = $data ? $data : '';
  }
}
