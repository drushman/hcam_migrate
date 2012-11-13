<?php
/**
 * @file langgo_migrate_taxonomy.keywords_grammar.inc
 *
 * Migrate langgo_source term from Drupal 6 to 7.
 */

/**
 * Migrate term.langgo_source to Drupal 7.
 */
class LanggoTermKeywordsGrammarMigration extends LanggoTaxonomyMigrateBase {
  public function __construct() {
    $arguments = array(
      'vocabylary'  => 'keywords_grammar',
      'src_vid'     => 8,
    );
    parent::__construct($arguments);
//    $this->description = 'Migrate term of "Keywords grammar" vocabulary from langgo_source to Drupal 7';
  }
}
