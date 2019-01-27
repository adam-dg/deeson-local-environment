<?php

/**
 * @file
 * Master module.
 */

$conf['master_version'] = 2;
$conf['master_allow_base_scope'] = TRUE;
$conf['master_current_scope'] = 'base';
$conf['master_modules']['base'] = array(
  // Core

<master_core>

  // Contrib

<master_contrib>

  // Custom & Features

<master_custom>
);

if (SETTINGS_ENVIRONMENT === D_ENVIRONMENT_LOCAL) {
  $conf['master_modules']['base'] = array_merge($conf['master_modules']['base'], array(
    // Core
    'dblog',
    'field_ui',
    'update',

    // Contrib
    'bean_ui',
    'mail_debugger',
    'mail_debugger_common',
    'memcache',
    'memcache_admin',
  ));
}

if (SETTINGS_ENVIRONMENT === D_ENVIRONMENT_DEV) {
  $conf['master_modules']['base'] = array_merge($conf['master_modules']['base'], array(
    // Core
    'dblog',
    'field_ui',
    'syslog',

    // Contrib
    'bean_ui',
    'entitycache',
    'memcache',
  ));
}

if (SETTINGS_ENVIRONMENT === D_ENVIRONMENT_TEST) {
  $conf['master_modules']['base'] = array_merge($conf['master_modules']['base'], array(
    // Core
    'dblog',
    'syslog',

    // Contrib
    'bean_ui',
    'entitycache',
    'memcache',
  ));
}

if (SETTINGS_ENVIRONMENT === D_ENVIRONMENT_PROD) {
  $conf['master_modules']['base'] = array_merge($conf['master_modules']['base'], array(
    // Core
    'syslog',

    // Contrib
    'bean_ui',
    'entitycache',
    'memcache',
    'warden',
  ));
}
