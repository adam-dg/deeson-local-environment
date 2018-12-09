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
