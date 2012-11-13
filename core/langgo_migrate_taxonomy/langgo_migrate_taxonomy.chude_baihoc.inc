<?php
/**
 * @file langgo_migrate_taxonomy.chude_baihoc.inc
 *
 * Migrate langgo_source term from Drupal 6 to 7.
 */

/**
 * Migrate term.langgo_source to Drupal 7.
 */
class LanggoTermChudeBaihocMigration extends LanggoTaxonomyMigrateBase {
  public function __construct() {
    $arguments = array(
      'vocabylary'  => 'chu_de_bai_hoc',
      'src_vid'     => 2,
    );
    parent::__construct($arguments);
//    $this->description = 'Migrate term of "Chu de bai hoc" vocabulary from langgo_source to Drupal 7';
  }
}
