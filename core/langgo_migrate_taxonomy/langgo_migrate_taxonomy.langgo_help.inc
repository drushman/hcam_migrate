<?php
/**
 * @file langgo_migrate_taxonomy.langgo_help.inc
 *
 * Migrate langgo_source term from Drupal 6 to 7.
 */

/**
 * Migrate term.langgo_source to Drupal 7.
 */
class LanggoTermLanggoHelpMigration extends LanggoTaxonomyMigrateBase {
  public function __construct() {
    $arguments = array(
      'vocabylary'  => 'langgo_help',
      'src_vid'     => 3,
    );
    parent::__construct($arguments);
//    $this->description = 'Migrate term of "Langgo Help" vocabulary from langgo_source to Drupal 7';
  }
}
