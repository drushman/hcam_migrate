<?php
/**
 * @file content/field.inc
 */

/**
 * Class to migrate content fields.
 */
abstract class HCNodeContentFieldBase extends DynamicMigration {
  protected $args;
  protected $sfi;
  protected $sfname;
  protected $sbundle;
  protected $stable;
  protected $df;
  protected $dfi;
  protected $dtable;

  public function __construct(array $args) {
    $this->args    = $args;
    $this->sfname  = $args['sfname'];
    $this->sbundle = $args['sbundle'];
    $this->sfi     = $this->sfiInfo();
    $this->df      = field_read_field($this->sfname);
    $this->dfi     = field_read_instance('node', $this->sfname, $this->sfi->type_name);
    $this->dtable  = "field_data_{$this->sfname}";

    parent::__construct(MigrateGroup::getInstance('Hcam'));
    $this->description    = "Migrate node.content.{$this->sftype}.{$this->sfname}";
    $this->dependencies[] = 'HCLanguage';
    $this->dependencies[] = 'HCInputFormat';
    $this->dependencies[] = 'HCUser';
    $this->dependencies[] = 'HCNodeTypes';

    // Find source table
    $this->stable = "content_type_{$this->sfi->type_name}";
    if (($this->sfi->instance_counter > 1) || $this->sfi->multiple <> 0) {
      $this->stable = "content_{$this->sfname}";
    }

    // Key Mapping
    $s_keys['vid'] = array('type' => 'int', 'unsigned' => TRUE, 'alias' => $this->stable);
    $d_keys = MigrateDestinationTable::getKeySchema($this->dtable);
    $this->map = new MigrateSQLMap($this->machineName, $s_keys, $d_keys);

    // Source, Dest, Mapping
    $this->source = new MigrateSourceSQL($this->query());
    $this->destination = new MigrateDestinationTable($this->dtable);
    $this->mapping();
  }

  protected function query() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select($this->stable);
    $select->innerJoin('node', 'node', "{$this->stable}.nid = node.nid");
    $select->addField('node', 'nid', 'nid');
    $select->addField('node', 'vid', 'vid');
    $select->addField('node', 'type', 'bundle');
    $select->condition('node.type', array('poll', 'product'), 'NOT IN');
    $select->condition('node.type', $this->sbundle);
    $select->orderBy("{$this->stable}.vid");
    foreach (array_keys($this->sfi->db_columns) as $k) {
      $select->addField($this->stable, "{$this->sfname}_{$k}");
    }
    db_set_active();
    return $select;
  }

  protected function mapping() {
    $this->addFieldMapping('entity_type')->defaultValue('node');
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('deleted')->defaultValue(0);
    $this->addFieldMapping('bundle', 'bundle');
    $this->addFieldMapping('entity_id', 'nid')
      ->sourceMigration(LanggoNodeHelper::getNodeBasicContentMigrations());
    $this->addFieldMapping('revision_id')->defaultValue(0);
    if ($this->sfi->multiple != 0) {
      $this->addFieldMapping('delta', 'delta');
    }
    else {
      $this->addFieldMapping('delta')->defaultValue(0);
    }
  }

  public function prepare($entity, stdClass $row) {
    $entity->revision_id = db_select('node')
        ->fields('node', array('vid'))
        ->condition('nid', $entity->entity_id)
        ->execute()
        ->fetchColumn();
  }

  /**
   * Get Field info
   */
  protected function sfiInfo() {
    db_set_active(HCAM_MIGRATE_DB_KEY);
    $select = db_select('content_node_field_instance', 'fi');
    $select->innerJoin('content_node_field', 'f', 'fi.field_name = f.field_name');
    $select->fields('fi');
    $select->fields('f');
    $instance_counter  = "SELECT COUNT(*)";
    $instance_counter .= " FROM {content_node_field_instance} fi2";
    $instance_counter .= " WHERE fi2.field_name = f.field_name";
    $select->addExpression("({$instance_counter})", 'instance_counter');
    $select->condition('f.field_name', $this->sfname);
    if ($sfi = $select->execute()->fetchObject()) {
      $sfi->widget_settings = unserialize($sfi->widget_settings);
      $sfi->display_settings = unserialize($sfi->display_settings);
      $sfi->global_settings = unserialize($sfi->global_settings);
      $sfi->db_columns = unserialize($sfi->db_columns);
    }
    db_set_active();
    return !empty($sfi) ? $sfi : FALSE;
  }

  /**
   * Construct the machine name from the source file name.
   */
  protected function generateMachineName() {
    return $this->args['machine_name'];
  }
}
