<?php
/**
 * @file langgo_migrate_main.comment.inc
 *
 */

/**
 * Migrate table block
 */
class HCCommentsMigration extends DynamicMigration {
  protected $args;
  protected $bundle;

  public function __construct(array $args) {
    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description = 'Migrate comments';

    $this->args    = $args;
    $this->bundle  = $args['bundle'];

    // Dependencies
    $this->dependencies[] = 'HCLanguage';
    $this->dependencies[] = 'HCInputFormat';
    $this->dependencies[] = 'HCUser';
    $this->dependencies[] = 'HCNodeTypes';

    // Key Mapping
    $s_keys['cid'] = array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE,);
    $s_keys['cid'] += array('description' => 'Source comment ID', 'alias' => 'c');
    $d_keys = MigrateDestinationComment::getKeySchema();
    $this->map = new MigrateSQLMap($this->getMachineName(), $s_keys, $d_keys);

    // Source
    $this->source = new MigrateSourceSQL($this->query());

    // Destination
    $this->destination = new MigrateDestinationComment($this->dtable);

    // Field Mapping
    $this->fieldMapping();
  }

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('comments', 'c');
    $select->fields('c', array('pid', 'nid', 'uid', 'subject', 'comment', 'hostname'));
    $select->fields('c', array('timestamp', 'status', 'format', 'thread'));
    $select->fields('c', array('name', 'mail', 'homepage'));
    $select->innerJoin('node', 'n', 'n.nid = c.nid');
    $select->condition('n.type', $this->bundle);
    db_set_active();
    return $select;
  }

  protected function fieldMapping() {
    $this->addFieldMapping('pid', 'pid');
    $this->addFieldMapping('nid', 'nid')
      ->sourceMigration(LanggoNodeHelper::getNodeBasicContentMigrations());
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration(array('HCUser'));
    $this->addFieldMapping('subject', 'subject');
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('mail', 'mail');
    $this->addFieldMapping('homepage', 'homepage');
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('hostname', 'hostname');
    $this->addFieldMapping('created', 'timestamp');
    $this->addFieldMapping('changed', 'timestamp');
    $this->addFieldMapping('status')->defaultValue(1);
    $this->addFieldMapping('thread', 'thread');
    $this->addUnmigratedDestinations(array('path'));
    $this->addUnmigratedSources(array('status', 'format', 'comment'));
  }

  public function getMachineName() {
    return "HCComment" . ucfirst($this->bundle);
  }

  public function prepare($entity, $row) {
    $entity->comment_body[LANGUAGE_NONE][0] = array(
      'value' => $row->comment,
      'format' => 'filtered_html',
    );
  }
}
