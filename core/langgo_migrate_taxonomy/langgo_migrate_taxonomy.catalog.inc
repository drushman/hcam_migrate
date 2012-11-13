<?php
/**
 * @file langgo_migrate_taxonomy.catalog.inc
 *
 * Migrate langgo_source term from Drupal 6 to 7.
 */

/**
 * Migrate term.langgo_source to Drupal 7.
 */
class LanggoTermCatalogMigration extends LanggoTaxonomyMigrateBase {
  public function __construct() {
    $arguments = array(
      'vocabylary'  => 'catalog',
      'src_vid'     => 1,
    );
    parent::__construct($arguments);
//    $this->description = 'Migrate term of "Catalog" vocabulary from langgo_source to Drupal 7';
  }
}
