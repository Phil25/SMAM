[![Build Status](https://travis-ci.org/Phil25/SMAM.svg?branch=master)](https://travis-ci.org/Phil25/SMAM)

# SourceMod Addon Manager

SourceMod Addon Manager (SMAM) is an automatic installer for [SourceMod](https://www.sourcemod.net/about.php) addons (plugins or extensions).

# Table of Contents
* [Idea](#idea-)
* [Features](#features-)
* [Security](#security-)
* [Installing](#installing-)
* [Examples](#examples-)
* [Help Menu](#help-menu-)

# Idea [^](#table-of-contents)
The idea of SMAM is to provide the community with automatic addon management, while at the same time not requiring developers to update their mean of distribution (AlliedModders, GitHub, or some other place). To achieve that, SMAM communicates with a web application, SMAMDB, hosted over at [smamdb.net](https://smamdb.net). SMAMDB is nothing more than a database holding information about where to download and install the specified addon files. SMAMDB is open source and can be viewed at [its GitHub page](https://github.com/Phil25/SMAMDB). And, if you're interested, you may read more about it on [its wiki](https://github.com/Phil25/SMAMDB/wiki).

# Features [^](#table-of-contents)
SMAM is a server-side tool that allows you to, per server directory:
* automatically install an addon,
* automatically remove an addon,
* browse installed addons,
* view information about installed addons, or
* search available addons online.

# Security [^](#table-of-contents)
1. SMAM, by default, fetches information from a database available at [smamdb.net](https://smamdb.net). Only selected users may directly add addons. Anything submitted by the community is curated and checked according to AlliedMods forums rules (open source, etc).
1. SMAM will prevent addons to install files anywhere outside the specified server directory (only in `(mod)/addons/` and down).
1. SMAM is not required to be run as root. If run, SMAM will stop unless specifically allowed by the user.
1. During removal, SMAM will not remove files that are shared by other addons which have been installed through SMAM.
1. SMAM has to know which server directory to act on. You may execute it from that directory for automatic detection, or specify it with `-d` option.

# Installing [^](#table-of-contents)
SMAM is still in development and no automatic installation is yet available. But you may download and compile the source code yourself, according to the instructions [here](https://github.com/Phil25/SMAM/wiki/Building).

# Examples [^](#table-of-contents)
command | action
-|-
`smam install tf2items` | install tf2items
`smam remove tf2items` | remove tf2items
`smam info` | view addons installed by SMAM
`smam info tf2items` | view information about a specific addon installed by SMAM
`smam install rtd` | `TODO: dependency resolution` install rtd and tf2attributes (its dependency)
`smam install rtd --no-deps` | `TODO: dependency resolution` install rtd only, without tf2attributes (its dependency)
`smam install tf2items -f` | force installing tf2items, overwrites files even if tf2items is already installed
`smam install tf2items -q --log ./file.log` | install tf2items quietly, log output in `./file.log`
`smam install tf2items -d /path/to/server/` | install tf2items in a specified server structure (`(mod)/addons/`)
`smam install mytestplugin --db-url localhost:8080` | install a plugin, information about which is fetched from `localhost:8080`

# Help Menu [^](#table-of-contents)
```
Usage:
  smam <command> [addons] [options]

Commands:
  install                              Install specified addons.
  remove                               Remove specified addons.
  search                               Search for addons.
  info                                 Get info about installed addons.

Options:
  -h [ --help ]                        Show help.
  -v [ --version ]                     Show version.
  -q [ --quiet ]                       Do not produce output.
  -f [ --force ]                       Force command execution.
  --no-prefix                          Disable prefixes in output.
  --no-color                           Disable color in output.
  --allow-running-as-root              Run SMAM even as root.
  --log arg                            Path to log file.
  -d [ --destination ] arg             Path to server.
  --db-url arg (=https://smamdb.net/)  URL of the database.
```
