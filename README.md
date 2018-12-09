# Deeson local environment and build pipelines installer and configurator

# Step 1 : Preparation

- Update the site configuration as per the [D7 Quickstart](https://github.com/teamdeeson/d7-quickstart).
- Files to pay particular attention to are `docroot/sites/all/conf/environment.inc` and `docroot/sites/default/settings.php`

# Step 2 : Installation

- Create the directory `docroot/sites/all/drush/deeson_local_environment`
- Change to that directory and clone the following Github repository `git@github.com:adam-dg/deeson-local-environment.git`
- Optionally remove the `.git` directory from the cloned repository

# Step 3 : Update

- From a terminal while in the repository root run the Drush command `drush dle-install`
- This will configure the boilerplate for Docker Compose, Bitbucket Pipelines and our build tools
- The tool will attempt to discover if you require any frontend build and output a tailored `.build.env`, `docker-compose.yml` and `bitbuclet-pipelines.yml`

# Step 4 : Tidy-up

- You WILL need to reinstate any prior customisations to `.env` such as changes to `PROJECT_NAME` and `PROJECT_BASE_URL`
- You MAY wish to review the `docker-compose.yml` file. Memcache is enabled by default but you may want to use Redis instead.
- You MAY wish to review `deployment-manager.json` and `bitbucket-pipelines.yml` if you wish to deploy branches other than `master`, `develop` and `UAT`
