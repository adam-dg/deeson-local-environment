<?php
/**
 * @file
 * Provide Drush integration for Deeson local environment configuration tool.
 */

/**
 * Implements hook_drush_help().
 */
function deeson_local_environment_drush_help($section) {
  switch ($section) {
    case 'drush:dle-install':
      return dt('Deeson local environment configuration tool.');
    case 'drush:dle-gm':
      return dt('Deeson local environment master module configuration generation tool.');
    case 'drush:dle-nd-prep':
      return dt('Deeson local environment Nodequeue downgrade preparation tool.');
    case 'drush:dle-nd-clean':
      return dt('Deeson local environment Nodequeue downgrade cleanup tool.');
    case 'drush:dle-nqv':
      return dt('Sets the schema version of Nodequeue installed.');
  }
}

/**
 * Implements hook_drush_command().
 */
function deeson_local_environment_drush_command() {
  $items = array();

  $items['deeson-local-environment-install'] = array(
    'description' => 'Deeson environment configuration tool.',
    'callback' => 'drush_deeson_local_environment_install',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap.
    'options' => array(),
    'examples' => array(
      'drush dle-install' => 'Deeson local environment configuration tool.',
    ),
    'aliases' => array('dle-install'),
  );

  $items['deeson-local-environment-generate-master'] = array(
    'description' => 'Deeson local environment master module configuration generation tool.',
    'callback' => 'drush_deeson_local_environment_generate_master',
    'options' => array(
      'custom-prefixes' => array(
        'description' => '.',
        'example-value' => 'foo,bar',
      ),
    ),
    'examples' => array(
      'drush dle-gm' => 'Deeson local environment master module configuration generation tool.',
    ),
    'aliases' => array('dle-gm'),
  );

  $items['deeson-local-environment-nodequeue-downgrade-preparation'] = array(
    'description' => 'Deeson local environment Nodequeue downgrade preparation tool.',
    'callback' => 'drush_deeson_local_environment_nodequeue_downgrade_prep',
    'options' => array(
    ),
    'examples' => array(
      'drush dle-nd-prep' => 'Deeson local environment Nodequeue downgrade preparation tool.',
    ),
    'aliases' => array('dle-nd-prep'),
  );

  $items['deeson-local-environment-nodequeue-downgrade-cleanup'] = array(
    'description' => 'Deeson local environment Nodequeue downgrade cleanup tool.',
    'callback' => 'drush_deeson_local_environment_nodequeue_downgrade_cleanup',
    'options' => array(
    ),
    'examples' => array(
      'drush dle-nd-clean' => 'Deeson local environment Nodequeue downgrade cleanup tool.',
    ),
    'aliases' => array('dle-nd-clean'),
  );

  $items['deeson-local-environment-nodequeue-downgrade-set-nodequeue-version'] = array(
    'description' => 'Sets the schema version of Nodequeue installed.',
    'callback' => 'drush_deeson_local_environment_nodequeue_downgrade_set_nodequeue_version',
    'options' => array(
      'version' => array(
        'description' => '.',
        'example-value' => '7201',
      ),
    ),
    'examples' => array(
      'drush dle-nqv' => 'Sets the schema version of Nodequeue installed to the default of 7201.',
      'drush dle-nqv 7200' => 'Sets the schema version of Nodequeue installed to the default of 7200.',
    ),
    'aliases' => array('dle-nqv'),
  );

  return $items;
}

/**
 * Deeson environment configuration tool.
 */
function drush_deeson_local_environment_install() {
  chdir(dirname(__FILE__));
  $command_path = getcwd();
  chdir('../../../../..');
  $root = getcwd();
  $temp = trim(`mktemp -d`);
  $clone = `git clone -b feature/revised-build-framework https://github.com/adam-dg/deeson-ci-quickstart-examples.git "$temp"`;
  `rsync -av --delete $temp/examples/drupal-7/scripts/ $root/scripts/`;
  `rsync -av $temp/examples/drupal-7/.env $root/`;
  `rsync -av $temp/examples/drupal-7/Makefile $root/`;
  `rsync -av $temp/examples/drupal-7/bitbucket-pipelines.yml $root/`;
  `rsync -av $temp/examples/drupal-7/composer.json $root/`;
  `rsync -av $temp/examples/drupal-7/deployment-manager.json $root/`;
  `rsync -av $temp/examples/drupal-7/docker-compose.yml $root/`;
  `rsync -av $temp/examples/drupal-7/drush.wrapper $root/`;

  // Copy .build.env.
  // Customise .build.env.
  $build_params = array(
    'drupal_version' => 7,
    'drupal_build' => 'Y',
    'drupal_build_php_tag' => 'wodby/drupal-php:7.1-2.4.5',
    'drupal_install_command' => './scripts/make/install-drupal.sh',
    'drupal_build_composer_install' => 'N',
    'drupal_fix_settings' => 'Y',
    'drupal_build_drush_make' => 'N',

    'frontend_build' => 'N',
    'frontend_dir' => './frontend',
    'frontend_node_modules' => '$frontend_dir/node_modules',
    'frontend_build_tag' => 'deeson/fe-node',
    'frontend_install_command' => '"npm install"',
    'frontend_build_command' => '"./node_modules/.bin/grunt build"',
  );
  $docker_compose_filename = 'no-frontend.docker-compose.yml';

  if (file_exists($root . DIRECTORY_SEPARATOR . 'composer.json')) {
    $build_params['drupal_build_composer_install'] = 'Y';
  }

  if (file_exists($root . DIRECTORY_SEPARATOR . 'drush-make.yml')) {
    $build_params['drupal_build_drush_make'] = 'Y';
  }

  if (is_dir($root . DIRECTORY_SEPARATOR . 'frontend')) {
    $build_params['frontend_build'] = 'Y';
    if (file_exists($root . DIRECTORY_SEPARATOR . 'frontend' . DIRECTORY_SEPARATOR . 'Gruntfile.js')) {
    $build_params['frontend_install_command'] = '"npm install"';
    $build_params['frontend_build_command'] = '"./node_modules/.bin/grunt build"';
      $docker_compose_filename = 'npm-grunt-frontend.docker-compose.yml';
    }
    elseif (file_exists($root . DIRECTORY_SEPARATOR . 'frontend' . DIRECTORY_SEPARATOR . 'gulpfile.js')) {
      $build_params['frontend_install_command'] = '"yarn install"';
      $build_params['frontend_build_command'] = '"yarn dist"';
      $docker_compose_filename = 'yarn-gulp-frontend.docker-compose.yml';
    }
    elseif (file_exists($root . DIRECTORY_SEPARATOR . 'frontend' . DIRECTORY_SEPARATOR . 'yarn.lock')) {
      $build_params['frontend_install_command'] = '"yarn install"';
      $build_params['frontend_build_command'] = '"yarn dist"';
      $docker_compose_filename = 'cdd-frontend.docker-compose.yml';
    }
  }

  $config_path = $command_path . DIRECTORY_SEPARATOR . 'conf' . DIRECTORY_SEPARATOR;
  $build_env = file_get_contents($config_path . '.build.env.template');
  foreach ($build_params as $build_param => $build_param_value) {
    $build_env = str_replace('<' . $build_param . '>', $build_param_value, $build_env);
  }
  file_put_contents($root . DIRECTORY_SEPARATOR . '.build.env', $build_env);

  // Can we customise docker-composer.yml and disable fe-node and fe-php if no frontend build detected?
  $docker_compose_file = $config_path . 'docker' . DIRECTORY_SEPARATOR . $docker_compose_filename;
  file_put_contents($root . DIRECTORY_SEPARATOR . 'docker-compose.yml', file_get_contents($docker_compose_file));

  // Can we customise bitbucket-pipelines.yml and disable frontend build step if no frontend build detected?
  $pipelines_file = $config_path
    . 'pipelines'
    . DIRECTORY_SEPARATOR
    . ($build_params['frontend_build'] == 'N' ? 'no-frontend.bitbucket-pipelines.yml' : 'bitbucket-pipelines.yml');
  file_put_contents($root . DIRECTORY_SEPARATOR . 'bitbucket-pipelines.yml', file_get_contents($pipelines_file));
}

/**
 * Deeson local environment master module configuration generation tool.
 */
function drush_deeson_local_environment_generate_master() {
  $master_base_exclude = array(
    'dblog',
    'field_ui',
    'syslog',
    'update',
  );

  $custom_extension_prefixes = explode(',', drush_get_option('custom-prefixes', ''));

  drush_include_engine('drupal', 'environment');

  $core = array();
  $contrib = array();
  foreach (drush_get_modules(FALSE) as $extension_name => $extension) {
    if (isset($extension->info['package']) && strpos($extension->info['package'], 'Core') === 0) {
      $core[$extension_name] = $extension;
    }
    else {
      $contrib[$extension_name] = $extension;
    }
  }

  ksort($core);
  ksort($contrib);

  $master_core = array();
  foreach ($core as $core_extension_name => $core_extension) {
    if (in_array($core_extension_name, $master_base_exclude)) {
      continue;
    }

    $master_core[] = '  '
      . (!$core_extension->status ? '//' : '')
      . "'" . $core_extension_name . "',";
  }

  $master_contrib = array();
  $master_custom = array();
  foreach ($contrib as $contrib_extension_name => $contrib_extension) {
    if (in_array($contrib_extension_name, $master_base_exclude)) {
      continue;
    }

    $custom = FALSE;
    foreach ($custom_extension_prefixes as $custom_extension_prefix) {
      if (stripos($contrib_extension_name, $custom_extension_prefix) === 0) {
        $custom = TRUE;
        break;
      }
    }

    if (!$custom) {
      if (!$contrib_extension->status) {
        continue;
      }
      $master_contrib[] = '  '
        . "'" . $contrib_extension_name . "',";
    }
    else {
      $master_custom[] = '  '
        . (!$contrib_extension->status ? '//' : '')
        . "'" . $contrib_extension_name . "',";
    }
  }

  chdir(dirname(__FILE__));
  $command_path = getcwd();
  chdir('../../../../..');
  $root = getcwd();

  $build_params = array(
    'master_core' => implode("\n", $master_core),
    'master_contrib' => implode("\n", $master_contrib),
    'master_custom' => implode("\n", $master_custom),
  );

  $config_path = implode(DIRECTORY_SEPARATOR, array($command_path, 'conf'));
  $master_inc = file_get_contents(implode(DIRECTORY_SEPARATOR, array($config_path, 'settings', 'master.settings.inc')));
  foreach ($build_params as $build_param => $build_param_value) {
    $master_inc = str_replace('<' . $build_param . '>', $build_param_value, $master_inc);
  }

  file_put_contents(implode(DIRECTORY_SEPARATOR, array($root, 'docroot', 'sites', 'all', 'conf', 'master.settings.inc')), $master_inc);
}

function drush_deeson_local_environment_nodequeue_downgrade_prep() {
  $qid_field = array(
    'description' => 'The primary identifier for a queue.',
    'type' => 'int',
    'unsigned' => TRUE,
    'not null' => FALSE,
  );

  // Get a list of queue machine names.
  $queues = db_select('nodequeue_queue', 'nq')
    ->fields('nq', array('name'))
    ->execute()
    ->fetchCol(0);

  $map = array();
  $i = 1;
  foreach ($queues as $name) {
    $map[$name] = $i++;
  }

  // Changes to nodequeue_queue table.
  db_add_field('nodequeue_queue', 'qid', $qid_field);
  _drush_deeson_local_environment_nodequeue_downgrade_populate_qid('nodequeue_queue', $map);

  // Changes to nodequeue_roles table.
  db_add_field('nodequeue_roles', 'qid', $qid_field);
  _drush_deeson_local_environment_nodequeue_downgrade_populate_qid('nodequeue_roles', $map);

  // Changes to nodequeue_types table.
  db_add_field('nodequeue_types', 'qid', $qid_field);
  _drush_deeson_local_environment_nodequeue_downgrade_populate_qid('nodequeue_types', $map);

  // Changes to nodequeue_subqueue table.
  db_add_field('nodequeue_subqueue', 'qid', $qid_field);
  _drush_deeson_local_environment_nodequeue_downgrade_populate_qid('nodequeue_subqueue', $map);

  // Changes to nodequeue_nodes table.
  db_add_field('nodequeue_nodes', 'qid', $qid_field);
  _drush_deeson_local_environment_nodequeue_downgrade_populate_qid('nodequeue_nodes', $map);

  // Change smartqueue table.
  if (db_table_exists('smartqueue')) {
    db_add_field('smartqueue', 'qid', $qid_field);
    _drush_deeson_local_environment_nodequeue_downgrade_populate_qid('smartqueue', $map);
  }
}

function drush_deeson_local_environment_nodequeue_downgrade_cleanup() {
  // Changes to nodequeue_queue table.
  db_drop_primary_key('nodequeue_queue');
  db_add_unique_key('nodequeue_queue', 'name', array('name'));
  db_add_primary_key('nodequeue_queue', array('qid'));
  db_change_field('nodequeue_queue', 'qid', 'qid', array(
    'type' => 'serial',
    'unsigned' => TRUE,
    'not null' => TRUE,
  ));

  // Changes to nodequeue_roles table.
  db_drop_index('nodequeue_roles', 'name');
  db_drop_field('nodequeue_roles', 'name');
  db_add_index('nodequeue_roles', 'qid', array('qid'));

  // Changes to nodequeue_types table.
  db_drop_index('nodequeue_types', 'name');
  db_drop_field('nodequeue_types', 'name');
  db_add_index('nodequeue_types', 'qid', array('qid'));

  // Changes to nodequeue_subqueue table.
  db_drop_index('nodequeue_subqueue', 'name');
  db_drop_field('nodequeue_subqueue', 'name');
  db_add_index('nodequeue_subqueue', 'qid', array('qid'));

  // Changes to nodequeue_nodes table.
  db_drop_index('nodequeue_nodes', 'name');
  db_drop_field('nodequeue_nodes', 'name');
  db_add_index('nodequeue_nodes', 'qid', array('qid'));

  // Change smartqueue table.
  if (db_table_exists('smartqueue')) {
    db_drop_primary_key('smartqueue');
    db_drop_field('smartqueue', 'name');
    db_add_primary_key('smartqueue', array('name'));
    db_change_field('smartqueue', 'qid', 'qid', array(
      'type' => 'serial',
      'unsigned' => TRUE,
      'not null' => TRUE,
    ));
  }
}

function drush_deeson_local_environment_nodequeue_downgrade_set_nodequeue_version() {
  $nodequeue_version = drush_get_option('version', 7201);
  db_update('system')
    ->fields(array('schema_version' => $nodequeue_version))
    ->condition('name', 'nodequeue')
    ->execute();
}

function _drush_deeson_local_environment_sort_extensions($a, $b) {
  return strcasecmp($a->info['name'], $b->info['name']);
}

function _drush_deeson_local_environment_nodequeue_downgrade_populate_qid($table, $queues) {
  foreach ($queues as $name => $qid) {
    db_update($table)
      ->fields(array('qid' => $qid))
      ->condition('name', $name)
      ->execute();
  }
}
