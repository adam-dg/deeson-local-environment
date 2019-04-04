# Deeson local environment and build pipelines installer and configurator

## Prerequisites

- Update the site configuration as per the [D7 Quickstart](https://github.com/teamdeeson/d7-quickstart).
- Files to pay particular attention to are `docroot/sites/all/conf/environment.inc` and `docroot/sites/default/settings.php`

## Installation

- Create the directory `docroot/sites/all/drush/deeson_local_environment`
- Change to that directory and clone the following Github repository `git@github.com:adam-dg/deeson-local-environment.git`
- Optionally remove the `.git` directory from the cloned repository

## Tools

### Installing the build tools framework 

- From a terminal while in the repository root run the Drush command `drush dle-install`
- This will configure the boilerplate for Docker Compose, Bitbucket Pipelines and our build tools
- The tool will attempt to discover if you require any frontend build and output a tailored `.build.env`, `docker-compose.yml` and `bitbuclet-pipelines.yml`

#### Post-installation tasks

- You WILL need to reinstate any prior customisations to `.env` such as changes to `PROJECT_NAME` and `PROJECT_BASE_URL`
- You MAY wish to review the `docker-compose.yml` file. Memcache is enabled by default but you may want to use Redis instead.
- You MAY wish to review `deployment-manager.json` and `bitbucket-pipelines.yml` if you wish to deploy branches other than `master`, `develop` and `UAT`

### Generating a configuration for Master module

- From a terminal while in the repository root run the Drush command `dle-gm`

### Nodequeue 7.x-3.x-dev downgrade to 7.x-2.x

- From a terminal while in the repository root run the Drush command `drush dle-nd-prep`
    - If this is to be run on a platform such as Acquia then you will want release this module to the platform and run it.
    - The preparation phase is non-destructive and just adds back the fields that 3.x-dev removes
- From a terminal while in the repository root run the Drush command `drush dl nodequeue --select`
    - Select to install your desired version of NodeQueue 2.x
- Update your features
    - If this is to be run on a platform such as Acquia then you will want release a code update at this point.
- From a terminal while in the repository root run the Drush command `drush dle-nd-clean`
- From a terminal while in the repository root run the Drush command `drush dle-nqv 7201`
- From a terminal while in the repository root run the Drush command `drush rr`
- From a terminal while in the repository root run the Drush command `drush updb -y`
- From a terminal while in the repository root run the Drush command `drush cc all`
