<?php
/**
 * @file content/field/emfield.inc
 */

/**
 *
 */
class HCNCFEmfield extends HCNodeContentFieldBase {
  protected function mapping() {
    parent::mapping();
    $this->addFieldMapping("{$this->sfname}_description", "{$this->sfname}_description");
    $this->addFieldMapping("{$this->sfname}_display", "{$this->sfname}_status");
    $this->addFieldMapping("{$this->sfname}_fid", "{$this->sfname}_value");
  }

  public function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = parent::query();
    $select->condition("{$this->stable}.{$this->sfname}_value", '', '<>');
    db_set_active();
    return $select;
  }

  public function prepare($entity, stdClass $row) {
    parent::prepare($entity, $row);

    try {
      $media = new MediaInternetYouTubeHandler();
      $media->embedCode = "http://youtu.be/" . $entity->{"{$this->sfname}_fid"};
      if ($file = $media->getFileObject()) {
        if (file_save($file)) {
          $entity->{"{$this->sfname}_fid"} = $file->fid;
        }
      }
    }
    catch (Exception $e) {}

    $entity->{"{$this->sfname}_fid"} = !empty($file) ? $file->fid : 0;
  }
}
