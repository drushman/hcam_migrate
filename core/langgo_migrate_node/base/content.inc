<?php

/**
 * Migrate basic node structure: Node, Revision, UID, Title, Body, …
 */
abstract class LanggoNodeBasic extends LanggoMigrateBase {
  protected $bundle;

  public function __construct() {
    parent::__construct();
    $this->dependencies[] = 'HCLanguage';
    $this->dependencies[] = 'HCInputFormat';
    $this->dependencies[] = 'HCUser';
    $this->dependencies[] = 'HCNodeTypes';
    $this->description = "Migrate basic node structure to Drupal 7";

    // Key Mapping
    $s_keys = $d_keys = MigrateDestinationNode::getKeySchema();
    $s_keys['nid']['alias'] = 'node';
    $this->map = new MigrateSQLMap($this->machineName, $s_keys, $d_keys);

    // Other mappings
    $this->destination = new MigrateDestinationNode($this->bundle);
    $this->source = new MigrateSourceSQL($this->query(), array(), $this->countQuery());
    $this->mapping();
  }

  protected function countQuery() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('node');
    $query->fields('node', array('nid'));
    $query->innerJoin('node_revisions', 'node_revisions', 'node.nid = node_revisions.nid');
    $query->condition('node.type', $this->bundle);
    db_set_active();
    return $query;
  }

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = $this->countQuery();
    $query->leftJoin('url_alias', 'url_alias', "CONCAT('node/', node.nid) = url_alias.src");
    $query->leftJoin('filter_formats', 'filter_formats', 'node_revisions.format = filter_formats.format');
    $query->fields('node', array('nid', 'type', 'language', 'uid', 'status', 'created', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'));
    $query->fields('node_revisions', array('title', 'body', 'teaser', 'format', 'log'));
    $query->addField('node_revisions', 'uid', 'revision_uid');
    $query->addField('node_revisions', 'timestamp', 'changed');
    $query->addField('url_alias', 'dst', 'path');
    $query->addExpression("REPLACE(LOWER(filter_formats.name), ' ', '_')", 'format');
    $query->addExpression("(SELECT COUNT(*) FROM {node} node2 WHERE node2.nid = node.nid)", 'revision');
    $query->orderBy('node_revisions.vid');
    db_set_active();
    return $query;
  }

  protected function mapping() {
    # $this->addFieldMapping('vid', 'vid');
    $this->addFieldMapping('tnid', 'tnid');
    $this->addFieldMapping('uid', 'uid')->getSourceMigration('HCUser');
    $this->addFieldMapping('revision_uid', 'revision_uid')->getSourceMigration('HCUser');
    $this->addFieldMapping('language', 'language');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'changed');
    $this->addFieldMapping('promote', 'promote');
    $this->addFieldMapping('sticky', 'sticky');
    $this->addFieldMapping('log', 'log');
    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('body', 'body');
    $this->addFieldMapping('body:summary', 'teaser');
    // @TODO: Check this
    $this->addFieldMapping('body:format', 'format')->getSourceMigration('HCInputFormat');
    $this->addFieldMapping('body:language', NULL)->defaultValue(LANGUAGE_NONE);
    // @TODO: Double check this
    $this->addFieldMapping('comment', 'comment');
    $this->addFieldMapping('path', 'path');
    $this->addFieldMapping('revision', NULL)->defaultValue(1);

    // @TODO: Don't know what's in Drupal 7
    // $this->addFieldMapping('???', 'translate');
    $this->addUnmigratedDestinations(array('is_new'));
    $this->addUnmigratedSources(array('nid', 'type', 'translate', 'moderate'));
  }

  public function nodeTypeInfo() {
    db_set_active(HCAM_MIGRATE_DB_KEY);

    $select = db_select('node_type');
    $select->fields('node_type');
    $select->condition('type', $this->bundle);
    if ($info = $select->execute()->fetchAll()) {
      $this->bundleInfo = $info;
    }

    db_set_active();
  }

  public function prepare($node, $row) {
    // Fix language
    $node->language = !empty($node->language) ? $node->language : LANGUAGE_NONE;

    // Fix revision
    $node->revision = $node->revision ? 1 : 0;

    if (!empty($node->body['0']) && !empty($node->body['arguments']['format'])) {
      $node->body = array(
        LANGUAGE_NONE => array(
          0 => array(
            'value'   => $node->body['0'],
            'summary' => $node->body['arguments']['summary'],
            'format'  => $node->body['arguments']['format'],
          )
        )
      );
    }
  }
}
