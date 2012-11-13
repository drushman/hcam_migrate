<?php
/**
 * @file langgo_migrate_taxonomy.tags.inc
 *
 * Migrate langgo_source term from Drupal 6 to 7.
 */

/**
 * Migrate term.langgo_source to Drupal 7.
 */
class LanggoTermTagsMigration extends LanggoTaxonomyMigrateBase {
  public function __construct() {
    $arguments = array(
      'vocabylary'  => 'tags',
      'src_vid'     => 4,
    );
    parent::__construct($arguments);
//    $this->description = 'Migrate term of "Tags" vocabulary from langgo_source to Drupal 7';
  }
}
