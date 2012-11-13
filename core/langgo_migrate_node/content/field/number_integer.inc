<?php
/**
 * @file content/field/filefield.inc
 */

class HCNCFNumberInteger extends HCNodeContentFieldBase {
  protected function mapping() {
    parent::mapping();
    $this->addFieldMapping("{$this->sfname}_value", "{$this->sfname}_value");
  }
}
