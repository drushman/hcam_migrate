<?php
/**
 * @file langgo_migrate_taxonomy.theloai.inc
 *
 * Migrate langgo_source term from Drupal 6 to 7.
 */

/**
 * Migrate term.langgo_source to Drupal 7.
 */
class LanggoTermTheloaiMigration extends LanggoTaxonomyMigrateBase {
  public function __construct() {
    $arguments = array(
      'vocabylary'  => 'the_loai',
      'src_vid'     => 5,
    );
    parent::__construct($arguments);
//    $this->description = 'Migrate term of "The loai" vocabulary from langgo_source to Drupal 7';
  }
}
