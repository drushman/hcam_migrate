<?php
/**
 * @file langgo_migrate_taxonomy.keywords_library.inc
 *
 * Migrate langgo_source term from Drupal 6 to 7.
 */

/**
 * Migrate term.langgo_source to Drupal 7.
 */
class LanggoTermKeywordsLibraryMigration extends LanggoTaxonomyMigrateBase {
  public function __construct() {
    $arguments = array(
      'vocabylary'  => 'keywords_library',
      'src_vid'     => 7,
    );
    parent::__construct($arguments);
//    $this->description = 'Migrate term of "Keywords library" vocabulary from langgo_source to Drupal 7';
  }
}
