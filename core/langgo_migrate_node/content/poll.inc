<?php
/**
 * @file langgo_migrate_main.poll.inc
 *
 * Migrate node.Poll from Drupal 6.
 */

/**
 * Class to migrate node.Poll from Drupal 6.
 */
class LanggoPollMigration extends Migration {
  public $filter_format_mapping = array();

  public function __construct() {
    parent::__construct();
    $this->description = t('Import poll from Langgo old site.');
    $this->dependencies[] = 'HCLanguage';
    $this->dependencies[] = 'HCInputFormat';
    $this->dependencies[] = 'HCUser';

    // Key Mapping
    $this->map = new MigrateSQLMap($this->machineName,
      array('nid' => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE)),
      MigrateDestinationNode::getKeySchema()
    );

    // Source
    $this->source = new MigrateSourceSQL($this->query(), array(), NULL, array('map_joinable' => FALSE));
    $this->destination = new MigrateDestinationNode('poll');
    $this->fieldMappings();
  }

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $query = db_select('node', 'n');
    $query->leftJoin('node_revisions', 'nr', 'n.nid = nr.nid AND n.vid = nr.vid');
    $query->leftJoin('filter_formats', 'ff', 'nr.format = ff.format');
    $query->leftJoin('poll', 'p', 'n.nid = p.nid');
    $query->fields('n', array('nid', 'uid', 'type', 'language', 'title', 'uid', 'status', 'created', 'changed', 'comment', 'promote', 'sticky', 'tnid'));
    $query->addField('nr', 'body', 'body_value');
    $query->addField('nr', 'teaser', 'body_summary');
    $query->addField('ff', 'name', 'body_format');
    $query->fields('p', array('active', 'runtime'));
    $query->condition('n.type', 'poll');
    $query->distinct();
    db_set_active();
    return $query;
  }

  protected function fieldMappings() {
    $this->addFieldMapping('uid')->defaultValue(1);
    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'changed');
    $this->addFieldMapping('language')->defaultValue('und');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('promote', 'promote');
    $this->addFieldMapping('sticky', 'sticky');
    $this->addFieldMapping('tnid', 'tnid');
    $this->addFieldMapping('comment', 'comment');
    $this->addFieldMapping('path', 'path')->defaultValue(NULL);

    $this->addFieldMapping('active', 'active');
    $this->addFieldMapping('runtime', 'runtime');
    $this->addFieldMapping('choice', 'choice');
    $this->addFieldMapping('votes', 'votes');
    $arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'body_summary'),
                                                    array('source_field' => 'body_format'),
                                                    array('source_field' => 'language'));
    $this->addFieldMapping('body', 'body_value')
         ->arguments($arguments)
         ->defaultValue('');
  }

  public function prepareRow($row) {
    // Transform body format to something we can use if it's not already.
    if (!filter_format_exists($row->body_format)) {
      $row->body_format = $this->transformFormatToMachineName($row->body_format);
    }

    // Get the original path, if any and make it available for mapping.
    $query_string = 'SELECT dst AS alias FROM {url_alias} WHERE src = :source';
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $url_alias = db_query($query_string, array(':source' => "node/{$row->nid}"))->fetchObject();
    if (!empty($url_alias->alias)) {
      $row->path = $url_alias->alias;
    }

    $choices =  db_select('poll_choices', 'c')
      ->fields('c', array('chtext', 'chorder', 'chvotes'))
      ->condition('c.nid', $row->nid)
      ->execute();
    $row->choice = array();
    foreach ($choices as $choice) {
      $row->choice[] = array(
       'chtext' => $choice->chtext,
       'chvotes' => $choice->chvotes,
       'weight' => $choice->chorder,
      );
    }

    $votes =  db_select('poll_votes', 'v');
    $votes->innerJoin('poll_choices', 'c', 'v.nid=c.nid AND v.chorder=c.chorder');
    $votes->condition('v.nid', $row->nid);
    $votes->fields('v', array('uid', 'hostname'));
    $votes->fields('c', array('chtext'));
    $votes->execute();
    $row->votes = array();
    foreach ($votes as $vote) {
      $row->votes[] = array(
       'chtext' =>$vote->chtext,
       'uid' => $vote->uid,
       'hostname' => $vote->hostname,
       'timestamp' => time(),
      );
    }
    db_set_active();
  }

  // Map friendly name of format (key) to machine name (format).
  public $format_mapping = array();

  public function transformFormatToMachineName($format) {
    // If we haven't initialized the $format_mapping, do it.
    if (empty($this->format_mapping)) {
      $result = db_query("SELECT format, name FROM {filter_format}");
      foreach ($result as $record) {
        $this->format_mapping[$record->name] = $record->format;
      }
    }
    if (!empty($this->format_mapping[$format])) {
      return $this->format_mapping[$format];
    }
    return 'filtered_html';
  }
}
