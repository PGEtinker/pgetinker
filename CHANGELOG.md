# Changelog

All notable changes to this project will be documented in
this file. Each "release" represents a batch of changes
at the date it was published and deployed on [PGEtinker](https://pgetinker.com).

Each release has notes that would be pertinent to the end user of the PGEtinker
website. For a comprehensive understanding of the evolution of the project,
please refer to the repository's commit history.

# <u>Release Notes - 2025-05-13</u>

## Notes

This release overhauls the way libraries are handled. The PGE version is no longer
tied to the versions of the other libraries. This simplifies library selection and
backend sanity checks. It also lightens the load when building the libraries as
fewer wasm objects are required for the same version combinations.

This release now includes newer versions of PGE up to v2.29, currently the latest
version available from OneLoneCoder!

This is also something of an experimental release that includes RayLib 5.5.

## Commits

### 2025-05-13

- main: ensure player html is empty if the player isn't running when layout reset
- main: set libraries cookie correctly on shares
- main: add update-code-hashes command to recalculate share hashes
- main: consolidate hashing of code and libraries, easier cache handling
- screenshot: use pgetinker-screenshot-ready event
- libraries: add pgetinker-screenshot-ready event to libraries
- main: add raylib to getCompilerLibraries
- main: add default raylib version
- main: add raylib code detector and handler to the compiler
- main: pge no longer requires special handling in librariesDialog
- libraries: remove PGE v2.30 entry from manifest
- main: pge no longer requires special handling
- language-server: pge no longer requires special handling
- libraries: more PGE compatibility changes
- libraries: tweak makefiles and make PGE compatibility changes
- libraries: recover PGE compile units
- libraries: recover PGE
- libraries: overhaul library handling

# <u>Release Notes - 2025-05-03</u>

## Notes

This release fixes a couple of bugs. First there are several complications
surrounding the pre-compiled libraries, in particular extensions that made
use of the 2d vector classes. The pre-compiled extensions have no way of
knowing when the geometry PGEX was in use and would end up with a dizzying
array of redefine/redeclaration errors that seem to come out of nowhere.

My solution to this problem was to restrict pre-compiled libraries to libraries
that do not make direct use of the v*2d classes or attempt to extend them in any
way. The exception to this is PGE itself, and PGE with the geometry definitions
baked into it! Anyways.. that problem should now be solved.

The next problem involves shares and library versions. The editor, settings, and
language server were blissfully unaware of a share's library versions. This is a
problem because the editor and the language server are at odds with what the code
expects to be compiled with. Sometimes it'll go un-noticed. Other times it will
break things in ways that aren't obvious to the end user. Now, the share forces
a change of library versions before the editor is fully loaded.

Finally, the compiler caching mechanism didn't account for changes in library
versions. Now any difference in code or library version will be a unique entry
in the cache.

## Commits

### 2025-05-03

- main: hash library versions, combine it with code hash to ensure cache keys account for library versions as well as code
- libraries: missed the sound pgex!
- pgetinker: whoops, check for NOT empty...
- libraries: due to heavy hassle with the v2d classes, no longer pre-compiling extensions
- pgetinker: fix empty forceMigrate handling
- main: remove "code" from route and views
- main: revamp share handling, use new get share api to set code and libraries
- main: add api to get a share by slug
- pgetinker: bump default language server memory to 1.5G
- pgetinker: add forceMigrate if update is called with ci

# <u>Release Notes - 2024-05-01</u>

## Notes

This release simplifies local deployments by removing the ``docker-compose.yml``
file from the git tracking. This will enable customizations without causing
conflicts when using the ``update`` command. Also, there's certain workflow
steps that are no longer needed that have been removed.

## Commits

### 2025-05-01

- github: long overdue removal of unused version handling steps
- github: remove step that takes down the services, we know they can't exist now (see PGEtinker/webhooks)
- github: remove cleanup that is not required since updated infra (see PGEtinker/webhooks)
- pgetinker: add remove docker-compose.yml, use docker-compose.sail.yml in initialize
- main: remove COMPOSE_FILE variable from .env.example
- main: add docker-compose.yml to ignored files
- main: move docker-compoose.yml to docker-compose.sail.yml
- main: add console command app:find-broken-shares
- main: remove unused console command app:identify-database
- main: remove unused console command app:compute-code-hashes
- main: remove unused FILE_RESOLVE from libraries example code
- main: update examples to use imgur hosted broken.png for now

# <u>Release Notes - 2024-04-25</u>

## Notes

This release fixes a few bugs in the infrastructure and with
the way shares were handled.

- Fix library_versions in shares. Wasn't applying them to the share.
- Added sanity check to the bulk screenshot command line utility
- Changed proxy image name to 'pgetinker/proxy', 'nginx' was colliding
with the official 'nginx' package and caused problems in unrelated
deployments using the 'nginx' image.

## Commits

### 2025-04-25

- main: include library_versions in share data.
- main: only grab a screenshot if it compiled successfully.
- pgetinker: change proxy image from 'nginx' to 'pgetinker/proxy'

# <u>Release Notes - 2025-04-18</u>

## Notes

This release fixes a bug where browser selected
themes weren't handled properly and caused styling
anamolies which, among other things, made the "What's New"
dialog look awful. 

PGEtinker now responds to and respects changes to
the browser theme.

## Commits

### 2025-04-18

- main: formatting
- main: add mardown.js to rollup
- main: handle browser theme changes
- main: apply theme in preload
- main: handle theme changes in markdown pages
- main: remove github-markdown.css
- main: add markdown dark and light styles

# <u>Release Notes - 2025-04-17</u>

## Notes

This release sees a fundamental change in the way PGEtinker
is developed. It is now organized into a 2 repositories and several
customized docker images and a script that function together to
provide the development experience.

See the <a href="https://github.com/PGEtinker/pgetinker" target="_blank" rel="noopener noreferrer">README.md</a>
file on the github repository for the quick start guide.

The purpose of this change was to make it a simple task to deploy a copy
of PGEtinker for local development. The following is a brief list of what
has changed, from the web user's perspective.

- Added library version selection.
- Added library versions example code.
- Changed emscripten stack size. Set to 128KB, up from 64KB.
- Added language server support for library version selection.
- Added olcPixelGameEngine v2.23 and associated repository headers.
- Added olcPixelGameEngine v2.24 and associated repository headers.
- Added olcPixelGameEngine v2.25 and associated repository headers.
- Added olcPixelGameEngine v2.26 and associated repository headers.
- Added olcPixelGameEngine v2.27 and associated repository headers.
- Added olcPGEX_GamePad from f5958a5.
- Added olcPGEX_MiniAudio v1.5.
- Added olcPGEX_MiniAudio v1.6.
- Added olcPGEX_MiniAudio v1.7.
- Added olcSoundWaveEngine v0.2.
- Added miniaudio 0.11.21.
- Fix hover effects artifacts when browser is zoomed.
- Changed settings dialog hover color.
- Changed to use flex layouts to make checkbox/labels look more sane.
- Added a more comprehensive changelog.
- Changed how release notes are displayed on the news dialog.
- Fleshed out system requirements and usage instructions on README.md

## Commits

2025-04-17

- pgetinker: add production docker compose script
- main: set sane defaults for .env.example
- main: add app:update-database cli to facilitate upgrading
- pgetinker: use effective memory limits
- language-server: set nsjail default logging to DEBUG
- Revert "main,language-server: add "pgetinker-compile" cgroups to nsjail configuration"
- Revert "app: add cgroup commands to startup script"
- Revert "language-server: add cgroup commnads to startup script"
- Revert "language-server: add "pgetinker-compile" cgroups to nsjail config"
- Revert "language-server: ensure cgroups are created no matter if it's server or command"

### 2025-04-15

- language-server: ensure cgroups are created no matter if it's server or command
- pgetinker: bump mem_limit and memswap_limit to 1.5gigs
- main: nsjail use "really quiet" mode in production environments
- language-server: nsjail use "really quiet" mode in production environments
- language-server: add "pgetinker-compile" cgroups to nsjail config
- language-server: add cgroup commnads to startup script
- app: add cgroup commands to startup script
- main,language-server: add "pgetinker-compile" cgroups to nsjail configuration
- main,language-server: set nsjail default logging to DEBUG
- main: log compiler status on failed health checks
- pgetinker: use simpler health check command
- language-server: do sanity checks when commands are run as well.
- language-server: update typescript
- language-server: update dotenv
- language-server: update ws
- language-server: update node
- pgetinker: add gitlog command to show the latest, unpushed commits relative to origin/develop
- pgetinker: add memory limits to the laravel.test service
- pgetinker: bypass service container for npm run build
- main: add Ubuntu 22.04 or later to system requirements in README.md
- main: typo in README.md
- main: add a stable internet connection to the list of system requirements
- main: fleshout release notes in changelog
- main: clarification of APP_URL limitations/known issues in README.md
- main: remove screenshots from the known issues as the screenshot service has been added.
- main: formatting README.md
- main: formatting README.md
- main: add more detailed explanation of scripts/pgetinker up -d in README.md
- main: add update instructions to README.md
- main: verbiage change Install PGEtinker, instead of Installation
- main: flesh out system requirements in README.md
- pgetinker: run development command for language server service
- pgetinker: add memory limits to the language server service
- pgetinker: move wait_for_healthy function so it can be used in initialize and update commands
- pgetinker: use exit instead of return
- pgetinker: database migration in update command
- pgetinker: switch to raw docker command instead of relying on pgetinker script to build emscripten libraries.
- pgetinker: switch to raw docker command instead of relying on pgetinker script for npm
- pgetinker: switch to raw docker command instead of relying on the pgetinker script for php composer
- pgetinker: update now requires services to be down
- main: use await on start/stop language client
- main: make setToExample async
- main: stopLanguageClient before setting example code then startLanguageClient after.
- main: add startLanguageClient to EditorPanel
- main: add stopLanguageClient to EditorPanel
- language-server: typescript config. skip library checks
- language-server: add npm build to the initializer script
- language-server: start production mode by default

### 2025-04-14
- main: update vite to version 6.2.6
- main: update axios to version 1.8.4
- pgetinker: add "update" command
- pgetinker: use APP_URL instead of hard coded url

### 2025-04-13

- main: correct typo in changelog
- main: whoops, use Changelog title for /changelog route
- main: use release notes instead of changelog
- main: add /release-notes endpoint
- main: add framed styles to markdown views and styles
- main: add framed handler to changelog route
- pgetinker: instruct use of detached mode
- pgetinker: use http cloning for wiki repository.
- app: clean up sanity_check_application
- main: older versions of PGE require different macros for geometry build test
- app: switch to apache2 instead of the php dev server
- main: readability
- proxy: remove unused wiki config
- proxy: resolve backend services at runtime instead of start-up

### 2025-04-11

- main: use menu background color for header, nav, and item links
- main: remove top margin/padding from first h1
- main: use background color from common style on markdown-body
- main: add html padding to markdown style
- main: use /changelog in iframe
- main: add iframe styles to dialog
- main: change overall background color

### 2025-04-10
- main: add github-markdown styles to the markdown view
- main: add github-markdown-css package
- main: add /changelog page
- main: add markdown view
- main: add markdown.scss
- main: use common styles between app and disagree pages
- main: add logging for app.version cache misses
- main: cache for 10 seconds for all deployments
- main: don't cut off version in newsDialog
- main: use app.version for issue url
- main: use window.AppVersion
- main: provide app.version to javascript via window.AppVersion
- main: provide app.version configuration

### 2025-04-09

- main: move /health-check route near other CodeController routes
- wiki: clean up wiki docker image/startup script
- main: add back button to librariesDialog
- main: use flex to make checkbox/labels look more sane
- main: use outline instead of border/margin for hover effects
- main: add a dialog hover color
- main: add back button to the dialog styles
- main: make conformStorage async, ensure the libraries manifest is available before proceeding
- main: make getLibrariesManifest async
- main: remove getLibrariesManifest from app.ts
- main: add library versions demo
- main: use globally stored librariesManifest
- main: preemptively load the libraries manifest, once
- main: readability
- main: flesh out librariesDialog
- main: libraries are mounted and shouldn't need configuration
- main: add librariesDialog to the settingsDialog
- main: fix a few style related nags
- main: add librariesDialog skeleton and supporting styles
- main: use text field in form buttons
- main: add text field for form buttons

### 2025-04-08

- main: add restartLanguageClient to the editor panel
- main: supabase codes don't have library versions, give them the latest
- main: use a share's libraries to compile it rather than the "latest"
- main: add library_versions to "codes" database and model

### 2025-04-07

- main: use "latest" library versions in GetShareThumbnails
- main: update app:move-database - move codes from supabase to the configured default database.
- main: configure emscripten stack size 128KB
- pgetinker: add a quick and easy clean for logs, workspace directories, and redis cache

### 2025-04-06

- main: add supabase database config to facilitate a future migration
- screenshot: remove noisy error message
- main: latest library versions should match the current live site versions until the proper mechanisms are in place to handle multiple versions

### 2025-04-05

- main: update volumes to use the shared emscripten installation
- pgetinker: add shared emscripten detection and pull to initialize.
- language-server: remove unused packages, remove emsdk, set volumes /opt/libs, /opt/emsdk
- app: remove emscripten and clangd, set volumes /opt/libs, /opt/emsdk
- main: add /shared/emsdk to ignored list
- libs: move libs from "services" to "shared" makes more semantic sense

### 2025-04-04

- screenshot: use puppeteer-core so save size
- screenshot: bump node to 23, use slim image, remove unused packages
- screenshot: reuse browser session across requests.
- main: report the message from the thrown exception

### 2025-03-30

- remove clunky library switcher, will revisit in the future
- remove error message from local screenshot block add logging for screenshot file
- remove unused function
- fix PWA manifest address, was trying for relative and broke on share urls
- add pma endpoint to proxy config
- add pma to docker-compose
- add pge-core-update event to all versions of PGE
- ignore screenshots directory
- allow locally stored screenshots if pit not provided
- screenshot service is now mandatory and nonconfigurable.
- add screenshot container

### 2025-03-28

- stop relying on env("APP_URL") for home templates
- dusk tests work now, remove issue from README.md
- remove debugging output from unit tests
- add browser testing to ci
- remove pre-emptive build step from ci
- ensure .env exists
- use APP_SERVICE instead of explicitly setting it locally
- stop relying on env("APP_URL") for share url generation

### 2025-03-27

- force URL generation to match the current request and not a static environment
- updated DuskTestCase
- set dusk to use the proxy service for the app url
- switch app to laravel.test in scripts, nginx.conf, and docker
- thanks Ciaran!
- add known issues to README
- at last, the README has installation instructions.
- remove the wiki removal from ci, turns out remove services/wiki from cache solved it.
- remove wiki from cache
- remove wiki repo after testing is completed.
- whoops, this one is Stop!
- remove interactive/tty from docker run commands
- ensure PGEtinker is down before initialize command
- build docker images prioer to initialize
- remove wiki during initialize
- add an actual first test
- add ci switch to initializer
- test 2
- test 1
- remove old workflows
- initialize sanity check for running and prompt to shutdown
- run checks in not "initialize"
- ignore /services/wiki
- clone wiki repo instead of using it as a submodule
- remove wiki submodule
- damn typos!!
- build emscripten libraries
- update branch on wiki submodule to be main
- fix permissions in wiki container
- add wiki symlinks and submodule handling to initializer script
- add wiki submodule
- if empty/unset, use latest libraries from manifest
- add latest version to libs manifest
- revamp vite config to conform to sail
- ignore services directory in typescript
- update environment templates
- remove unused testing environment template
- update package-lock.json
- remove unused nsjail configurations and update ignored files
- add libraries
- add language server service
- add custom sail script to manage and develop pgetinker locally
- add sail docker container contexts
- remove old docker files
- update laravel and vendor stuff

### 2025-03-04

- rename duplicate class name
- update vite
- add localhost hmr/hosting config
- update default libs directory

### 2025-02-23

- add volumes, remove emscripten and libraries from inside the container
- do a whole lot of  sanity checks on container startup
- add app:identify-database to facilitate container startups
- specific debug config for nsjail
- include compiler and linker commands in the compiler logs
- add avater asset
- delete library script

### 2025-02-08

- overhaul to support changing of library versions

### 2024-09-15

- add validateLibraries to CodeController
- add auth.session to the libraries endopoint
- move nsjail configuration to misc
- nsjail working directory = /workspace instead of /user to match language client/server
- use getCompilerLibraries in Compile and Share
- add getCompilerLIbraries

### 2024-09-14

- verbiage
- add PGETINKER_LIBS_DIRECTORY default environment config
- add libraries api endpoint
- add version selector
- move form rendering function to their own file
- add library version configuration to the storage conforming function
- move shell and nsjail configuration out of third_party into misc
- old libraries

# <u>Release Notes - 2024-09-06</u>

## Notes

- Fixed [Issue #122](https://github.com/PGEtinker/pgetinker/issues/122)

## Commits

### 2024-09-06

- fix the input.rename-input dark on dark coloring problem
- add the model service override to fascilitate the text models

# <u>Release Notes - 2024-07-12</u>

## Notes

- Fixed [Issue #120](https://github.com/PGEtinker/pgetinker/issues/120)

## Commits

### 2024-07-12

- set csrf cookie before calling /api/news
- set csrf cookie before calling /api/supporters
- set csrf cookie before calling /api/compile and /api/share
- set csrf cookie before starting monaco editor
- set csrf cookie before language client retry connection

# <u>Release Notes - 2024-07-07</u>

## Notes

- Fixed bug involving shared code persistence
- Changed threshold for responsive navbar 
- Added session guard to compile and share API
- Added session guard on language client

## Commits

### 2024-07-07

- use cookies for auth

### 2024-07-06

- formatting
- add session auth guard
- add statefulApi middleware

### 2024-07-05

- clear shared code container
- change threshold for responsive navbar
- render logo
- clear shared flag when editor is changed

# <u>Release Notes - 2024-06-30</u>

## Notes

- Fixed [Issue #111](https://github.com/PGEtinker/pgetinker/issues/111)
- Fixed [Issue #112](https://github.com/PGEtinker/pgetinker/issues/112)
- Added logo graphic

## Commits

### 2024-06-30
- pageshow event to check if we got her by history, if so. force reload
- add logo graphic
- use assign on disagree, fixes page loading issues.
- fix broken test
- fix spelling error in terms dialog

# <u>Release Notes - 2024-06-29</u>

## Notes

- Added example code menu to navbar
- Added example code menu to mobile navigation
- Added PGEtinker Classic Demo to the example code menu
- Added Bare olcPixelGameEngine to the example code menu
- Changed default code to PGEtinker Classic Demo example code
- Removed default code button from settings dialog
- Removed unused api endpoint for default code
- Removed unused function to retrieve default code
- Fixed fieldId should always refresh with each instance of the settings dialog
- Added button to a form group to make the width uniform on the settings dialog
- Changed select now has label and value rather than just label

## Commits

### 2024-06-29

- remove defunct reference in README.md
- remove tests/Unit from test suite as there's no tests there
- remove example 3 to 6 from example menu

### 2024-06-28

- remove example test
- remove model and tests pertaining to it, obselete since LSP
- fix misaligned submenus
- fix broken browser test redux
- fix broken test
- fix using example code
- example2 is now a bare PGE project
- example1 is now the PGEtinker classic example
- use innerTExt for the code name, switch example to code-id
- use template in mobile example menu
- use single template for example menu, easier to keep it all straight
- revamp setToExample to use codeId and codeName instead of using metadata from example loader
- don't add  metadata to the example code loader

### 2024-06-27

- update build scripts
- move miniaudio.c to .cpp to silence annoying log
- dusk test config
- change default code to example code and make appropriate changes

### 2024-06-26

- load examples when they're clicked
- add examples to the mobile menu dialog
- a whole lot of unused code
- forgot a little event prevent default!
- dismiss dialog before calling share
- dismiss dialog before calling supporters dialog
- dismiss dialog before calling news dialog
- dismiss dialog before calling settings dialog
- remove example code stuff from settings dialog
- add lightbulb icon
- add examples menu/submenu
- add example codes
- add example codes
- add setToExample function to quickly switch to an example code
- switch to new examples code
- add examples menu to navbar styles
- extend time allowed for share dialog to appear
- add 5 and 6
- replace problematic macro in example codes
- update browser testing to account for updated UI elements
- fieldId should always refresh with each instance of the dialog
- replace default code button with drop down
- add description to default layout button
- add button to form group in order to make consistent width, add description
- still not sure what changed, but it changed?
- select has label and value instead of simply just a value
- add examples 2 through 4
- stop using localStorage, use setStorageValue instead
- switch default code to example1.cpp and remove unused api calls

# <u>Release Notes - 2024-06-25</u>

## Notes

- Fixed light theme persistence
- Added link icon to links menu item
- Added help to menu
- Added patreon icon to supporters menu item
- Added styles to account for space used by help menu item
- Added preloader that actually lives up to it's name!

## Commits

### 2024-06-25

- remove unused imports
- update browser tests to account for a working preloader
- revamp theme updater
- add actual preloader!
- call reconnectHandler immediately
- loading container defaults to flex
- important on .hidden class
- account for help menu for navbar styling
- add help to menu
- add link icon
- add patreon icon
- add help, link, and patreon icons
- fix light mode persistence

# <u>Release Notes - 2024-06-19</u>

## Notes

- Fixed [Issue #105](https://github.com/PGEtinker/pgetinker/issues/105)
- Fixed other dialog styles for the portrait layout

## Commits

### 2024-06-19

- share dialog URL and button styles for portrait layout
- make supporters styles better all around
- footer styles in portrait layout
- footer button styles for portrait layout
- heading font size in portrait layout

# <u>Release Notes - 2024-06-17</u>

## Notes

- Fixed [Issue #98](https://github.com/PGEtinker/pgetinker/issues/98)
- Removed sponsor spot from navbar
- Added normalize.css
- Added link to Javid's youtube channel
- Changed navbar menu, made ready for mobile
- Added mobile menu
- Added settings dialog
- Changed dialog behavior, no more click anywhere!
- Added toast notifications
- Added Javid Mode to settings dialog
- Added editor.inlayHints to settings dialog
- Fixed browser tests, broke due to UI changes
- Changed screenshot fail graphic (Thanks TechnicJelle)
- Changed light theme, make dialogs easier on the eyes
- Fixed [Issue #101](https://github.com/PGEtinker/pgetinker/issues/101)
- Fixed [Issue #100](https://github.com/PGEtinker/pgetinker/issues/100)

## Commits

### 2024-06-17

- fire compile success handler before calling the share dialog
- use javascript "version" variable instead of attempting to use blade in javascript
- fix zoom reset keybinding
- tweak light theme
- buttons no longer change colors based on theme
- revamp screenshot fail graphic (Thanks Technic)
- update browser tests for ui changes
- add editor.inlayHints configuration to settings dialog
- switch setTheme to updateConfiguration
- add Javid Mode
- compiler toasts only in portrait layout
- patreon functions whether a redis cache is available or not
- move settings toasts to the settings dialog
- refactor and add toasts

### 2024-06-16

- toastify
- add margin to submenu items
- add utility styles
- actually use scss in compiler output panel
- actually use scss in console panel
- actually use scss in editor panel
- actually use scss in player panel
- update share dialog, remove click anywhere, add footer, add close button
- remove anywhere click handler, add close button to footer, use less specific variable name for the dialog
- return a promise from rendering function, remove click anywhere handler, add close button to footer
- reorganize imports
- revamp navbar in html template
- add mobile and settings dialogs
- revamp scss to actually use scss features and make it easier to read
- reword to use less specific names for the dialogs, and remove after resolve
- supporters dialog should resolve
- reorganize styles
- add youtube icon
- fix share dialog in narrow viewports
- darker dialog background
- add normalize.css

### 2024-06-15

- remove the sponsor entry from the navbar
- reorganize scss entries in vite config
- re organize scss files

### 2024-06-11

- patch lc reconnection handler

# <u>Release Notes - 2024-06-10</u>

## Notes

- Added language client automatic reconnect
- Changed to typescript, partially
- Removed glyph margin from line number gutter

## Commits

### 2024-06-10

- test was flawed to expect the C++ Editor and Emscripoten Player, should be News and Updates
- fix language client failure to connect hanging application loading.
- remove glyph margin from line number gutter
- remove unused argument
- use more default vite settings for a typescript project
- convert to typescript phase 2
- convert typescript phase 1 + language client reconnect

# <u>Release Notes - 2024-06-09</u>

## Notes

- Fixed [Issue #92](https://github.com/PGEtinker/pgetinker/issues/92)
- Fixed default layout button handler
- Removed emscripten cache messages from compiler output

## Commits

### 2024-06-09

- remove cache messages from compiler output
- fix default layout button handler
- truncate version to 7

# <u>Release Notes - 2024-06-08</u>

## Notes

- Fixed mobile oversize issue (big win for mobile)
- Added diagnostics middleware to monaco language client
- Changed Build Information panel to Compiler Output
- Added Problems panel [Issue #85](https://github.com/PGEtinker/pgetinker/issues/85)
- Removed old default layout
- Added default layout for portrait
- Added default layout for landscape
- Added storage abstraction
- Fixed [Issue #88](https://github.com/PGEtinker/pgetinker/issues/88)
- Added [Issue #87](https://github.com/PGEtinker/pgetinker/issues/87)

## Commits

### 2024-06-08

- add badges for error, warning, info, and hint counters to the problem panel tab
- gracefully continue on, when Redis connection fails
- set TTL for compiler caches
- set TLL in remote include cache
- adjust browser tests to match new ui elements
- only change to console on the first run of the program
- set focus to editor and problems panels
- make problems panel focused when it's state changes.
- make problems cells of specific width, so as to not change very time a new entry is added/removed
- remove moros shaming, the task is complete
- use new storage, add new layouts, add problems panel, add compiler output panel, remove info panel
- default layouts for portrait and landscape modes
- new storage system
- remove old default layout
- add problems panel
- use new storage method in editor/code
- refactor console panel
- add compiler output panel
- remove info panel
- gut restart attempt

### 2024-06-07

- let monaco hover hints overlap the layout

### 2024-06-06

- add guards if the monaco wrapper hasn't been started
- fix mobile oversize

### 2024-06-05

- shame on moros if he leaves this here
- wrapping pre-formatted text for the info panel after builds
- pretty looking errors in the info panel
- wip problems in the #info-panel
- handleDiagnostics middleware
- set the cursor position and set editor focused in the reveal function
- add focus to editor
- remove preformatted style
- add restart after disconnect

# <u>Release Notes - 2024-06-04</u>

## Notes

- Added Alert when a runtime error occurs

## Commits

### 2024-06-04

- remove unused monaco
- remove unused editor panel functions, add compiler info on success handler
- show an alert when a runtime error is signalled
- add runtime error message from emscripten shell
- remove console from emscripten shell

# <u>Release Notes - 2024-06-03</u>

## Notes

- Added Clangd Language server to the backend
- Added Language client and related packages to the frontend
- Added Narrow screen tweaks for mobile users (first step)
- Added Donation link
- Added An easy way to set a panel to have focus, required for an upcoming mobile update
- Added Loading screen
- Removed "show-console" hook to the emscripten template, no longer used
- Fixed Overzealous auto complete
- Fixed Linker error when the Geometry utility was included after the OLC_PGE_APPLICATION macro

## Commits

### 2024-06-03

- disable inlay hints
- break detecting implementations into 2 phases, first detection, second application
- patch share redirections for staging
- patch sharedFlag for staging
- well this could have been bad

### 2024-06-02

- relax requirement to see the emscripten player
- account for the new loading screen in the tests
- Merge branch 'add-miniaudio-and-olcPGEX_MiniAudio' into intellisense!!!
- add progress web application
- keybindings
- add donation link
- first step into responsive
- add id to settings menu, to aid in responsive layout
- todo: responsive layout
- set active tab Editor Panel on load
- set active tab Editor Panel when compile fails
- set active tab Editor Panel when stopped
- set active tab Player Panel when RUN
- setActiveTab by the title
- remove "show-console" event in player
- replace focus related functionality in info panel
- remove unused "shown", replace focus related functionality
- add miniaudio/olcPGEX_MiniAudio
- add editor service
- add loading screen
- async/await set editor theme
- remove unused imports
- move loading animation to loading.scss
- revamp UpdateTheme
- wait for the editor to finish initializing
- first working editor/wrapper
- add esbuild-import-meta-url-plugin
- adding a whole lot of vscode/monaco packages
- bypass special characters, this is meant to be code after all
- make styles/variable impossible to conflict with monaco/vscode

### 2024-05-31

- gut monaco editor


# <u>Release Notes - 2024-05-28</u>

## Notes

- Fixed [Issue #78](https://github.com/PGEtinker/pgetinker/issues/78)

## Commits

### 2024-05-28

- money where the mouth is, make damn sure it works.
- use regex to determine the validity of the implementation macro, if any
- public processCode method, so testing is possible
- add getCode method

# <u>Release Notes - 2024-05-27</u>

## Notes

- Changed complete revamp of the frontend code, much more organized
- Fixed UI annoyances
- Added Control+S to the Build and Run command
- Added Default editor font size
- Added Control+Mouse Wheel zooming in the editor
- Added Control+0 to reset editor zoom
- Changed Build &amp; Run to Run, that turns into a stop button when the player is running

## Commits

### 2024-05-27

- spit up shows and dismisses
- dusk needs to start maximized
- changed filename to BrowserTest and updated test to match new button
- start and stop button, and everything related
- add keyboard binding to reset zoom
- add default font size, enable mouse wheel zooming
- add Ctrl+S to the Build and Run command keybindings
- restore monaco intellisense settings
- remove unused monaco workers

### 2024-05-26

- remove beta subpath, not going to use it
- complete reorganize

# <u>Release Notes - 2024-05-25</u>

## Notes

- Fixed screenshot failure handling
- Changed example code to use image hosted in the pit, rather than imgur

## Commits

### 2024-05-25

- add "changed" to news/changelog dialog
- swap example code to use image hosted on the pit, rather than imgur
- return failed graphic on failed response, response->body on success
- return failed screenshot graphic instead of "null"
- better default failed screenshot

# <u>Release Notes - 2024-05-23</u>

## Notes

- Added screenshot thumbnails for share embeds
- Fixed [Issue #73](https://github.com/PGEtinker/pgetinker/issues/73)

## Commits

### 2024-05-23

- add special test for geometry utility
- add special geometry build of PGE
- apply special rule for geometry header when PGE implementation is detected
- find geometry header in code
- better compiler output logs
- update README.md with attribution for placeholder failed screenshot
- failed screenshot graphic
- don't attempt to upload if no valid image content has been set
- return failed screenshot when the screenshot service fails to return an image in time

### 2024-05-22

- remove focus hijack from replaced PGE
- one-off command to get share thumbnails for existing shares
- block use of this command (remove later?)
- log when attempting to get a thumbnail on an existing share
- report when a screenshot attempt fails

### 2024-05-21

- put og:description in
- add placeholder console utility to update thumbnails of existing shares
- add thumbnail embed tags
- somehow new PGE made it into the commits, whoops
- if the share's thumbnail isn't set, try to set it
- send in the bottttttts
- add debug info to pit uploader
- guard pit
- guard screenshotter
- add taking thumbnails to the share logic
- add thumbnail to share dialog
- migration add thumb_url to codes table
- upload file to pit
- take screenshot from html


# <u>Release Notes - 2024-05-20</u>

## Notes

- Fixed [Issue #70](https://github.com/PGEtinker/pgetinker/issues/70)
- Fixed PGE focus hijacking.

## Commits

### 2024-05-20
- mouse wheel handler to turn off auto scrolling
- auto scroll button handler to turn auto scrolling back on
- add condition to the console auto scroller
- fix PGE hijack
- add auto scroll button element and style
- add utility style to hide an element

# <u>Release Notes - 2024-05-17</u>

## Notes

- Added support to deploy on subpaths
- Added updated version of PGE, Extensions, and Utlities
- Fixed patreon supporters not updated after PGEtinker upgrades
- Added filters for compiler output

## Commits

### 2024-05-17

- get raw output for compiler log file
- add filtering to getErrorOutput
- add filtering to getOutput
- remove unused function in CodeController
- remove unused references
- show the dialog, whether the api call succeeds or fails.
- return $supporters from getPatreonNames
- move get_supporters api endpoint to the PatreonController
- use redis instead of filesystem
- yet another try to get redis in the dockerfile
- add redis to docker container
- only attempt to use redis if compiler caching is set
- update geometry2d and changelog
- use v0.02 libs in compiler and app
- update example environment config
- update test environment config
- use redis for caching instead of filesystems
- use redis for cache instead of filesystems
- add getStatus
- add deserialize
- add serialize

### 2024-05-16

- add v0.02 libs
- re-add docker ignore file
- formatting
- use absolute url for favicon
- check for, and set axios default baseURL, beta/staging
- use APP_URL in shareURL
- force root url to APP_URL
- vitejs relative base

# <u>Release Notes - 2024-05-14</u>

## Notes

- Added view counter to shared codes

## Commits

### 2024-05-14

- configure NS_JAIL for CI
- increment view_count on code and save it
- add migration for view_count
- update workflow
- learning things is important..... i take back all the thoughts i had about docker

# <u>Release Notes - 2024-05-12</u>

## Notes

- Added compiler existence check
- Added health check

## Commits

### 2024-05-12

- add health-check api endpoint
- test the compiler health check
- compiler healthCheck - see that it exists!

### 2024-05-11

- rip off the docker band-aid

# <u>Release Notes - 2024-05-08</u>

## Notes

- Added limitations to the remote include feature
- Catch compiler/linker timeout for better error handling

## Commits

### 2024-05-08

- catch compiler and linker command timeout
- allow remote includes of http as well as https
- add early break if duration is too long
- use REMOTE_INCLUDE_CACHING instead of COMPILER_CACHING env variable
- switch from Http::get to  PendingRequest to add timeouts
- add default processing timeout

# <u>Release Notes - 2024-05-06</u>

## Notes

- Added cloudflare analytics and disclosure
- Added supporter wall
- Fixed bug with supporter wall
- Fixed security flaw. iframes weren't created with sandbox

## Commits

### 2024-05-06

- ensure iframes are created with a sandbox
- data is object, not simple array of strings
- change class name of new console command
- use controller function instead
- move api into a controller
- console command to get names from patreon
- support empty list of subscribers
- supporter dialog styles
- add supporter link
- remove duplicates that were there for testing
- fix empty subscribers list
- add supportersDialog
- add patreon supporters api
- disclose analytics
- add patreon token to example env
- remove unused variables
- add cloudflare web analytics
- add database logger
- helpful log messages
- stderr in order to display the error to the user
- revampe compileCode to use local disk for compilation and remote disk for persistence

# <u>Release Notes - 2024-05-05</u>

## Notes

- Added logging to database
- Added s3 bucket storage system
- Fixed hashCode string literal test

## Commits

### 2024-05-04

- fix typo that could have affected test results
- use s3 bucket for cache, if configured
- remove unused filter code
- throw when FS fails
- install deps for s3 bucket storage

# <u>Release Notes - 2024-05-02</u>

## Notes

- Fixed [Issue #50](https://github.com/PGEtinker/pgetinker/issues/50)
- Added Build Information panel (Requires you to reset to default layout)
- Added Console panel (Requires you to reset to default layout)

## Commits

### 2024-05-02

- update test to reflect the changed UI
- default layout closes player iframe, restore it if we have html
- fix overflow problems
- change order of buttons
- ensure new panels exist in the layout before using them
- allow stacks to scroll
- exclude A tags from the anywhere click handler
- parse markdown urls in news API
- fix compilerRegex and properly set severity'
- set info panel active during compile
- formatting
- add handler for console output
- add transmitter for console output
- remove handler for non-existant button
- add scaffolding for the new layout elements
- scroll to the top of the editor when loading the default code
- change filter
- truncate hash for issue link
- doesn't belong in dockerfile, belongs in entrypoint
- fix dockerfile: clear all of the cached everything
- unit tests status badge
- environment needs to be moved first, otherwise it's not available after build
- fix workflows
- seriously, what the hell
- clean up after git
- try using git directly, consolidate image management

# <u>Release Notes - 2024-05-01</u>

## Notes

- Added the changelog
- Added the first changelog entry
- Added version indicators in frontend/backend
- Added versioning to the container image build process
- Added Pixel Code font
- Added news and update dialog (you're seeing it right now!)
- Removed unused assets

## Commits

### 2024-05-01

- separate tags
- complete redo of the docker builder functions
- try moving it from within the container
- add environment variables to example, before copying
- try outputting another way
- more attempts at fixing the workflow
- try again
- god damnit
- try to use comma separated and pray the variable works
- fix bad command in workflow
- add news and updates to the browser tests
- halve the delay for the handlers
- get the changelog, dynamically
- changelog entry inline, not block
- news api, parse latest batch of changes in the changelog
- add the changelog and the first entry
- set version during build, use git commit hash
- show news and update dialog when there are new changes, or when the new menu item is clicked
- add news and update dialog
- add version to bug reporting
- frontend versioning
- styling consistency
- remove unused images
- add Pixel Code font and use it!

# <u>Pre-Release Notes</u>

## Notes

Everything from this point on is from a time when
PGEtinker was still very much in flux. Many things
were changing and nothing was stable. Lots of changes
were made very quickly and were not documented particularly
well. Read on for amusement or horror, your choice.

## Commits

### 2024-04-28

- mover console command and development database connection
- remove trigger
- dockerhub on dispatch
- trigger #1
- build dockerhub image on push to main
- install pqsql for production

### 2024-04-27

- update workflows, start the server before browser tests
- update tests
- god damnit!
- take advantage of all of this wonderful self-hosted goodness
- the big refactor
- dusk browser testing

### 2024-04-26

- break out the share dialog into it's own file
- break out the agree dialog into it's own file
- automatically change versions every time npm dev and build scripts are run
- delete
- fix monaco

### 2024-04-25

- add build-and-run action to monaco, and bind it to Ctrl+Enter
- PGE emscripten forces focus, probably to ensure sound on load, but it's bad for PGEtinker
- add custom error pages (well only 503)
- fix error handling and catch 503 error
- add custom artisan command to recompute code hashes
- close share dialog and copy the text when clicking anywhere
- new compiler indicators
- make unhandled errors give a generic error urging the user to make a bug report
- don't remove all elements when clicking disagree

### 2024-04-24

- verbiage
- flesh out disagree
- navigate to the /disagree when clicking the I disagree button
- add disagree.scss
- add disagree route/view
- add supporters link placeholder
- add patreon link
- javidx9 is first sponsor, confirmed
- first sponsor slot in layout/css
- update README.md to reflect new test badges
- simpler name for the dockerhub workflow
- flesh out workflows
- flesh out the tests for the new hash

### 2024-04-23

- make tests consistent
- tokenize the code before hashing
- make a test
- add include via https
- fix some logger issues, add check for remoteIncludeCache directory, create if missing

### 2024-04-22

- fix breaking apostrophe
- load model.h so it can be used for completion
- monaco defaults configuration to enable intellisense-like functionality
- remove scratch, text file
- script builds consolidated model.h file, ignores it
- backend provides the model
- persist console state between builds and between browser sessions
- refactor theming
- flesh out navbar and add icons
- localize padding settings on dialogs
- add navbar to header, remove menu from editor panel

### 2024-04-21

- Update issue templates
- add badge to README.md
- Update after-unit-tests.yml
- Create after-unit-tests.yml
- update README
- Update and rename laravel.yml to unit-tests.yml
- build libs in test workflow
- first compiler test, of many!
- delete example test
- delete old manual  http testing
- Update laravel.yml
- Create laravel.yml
- Delete .github/workflows directory
- Update laravel.yml
- Update laravel.yml
- trigger gh action
- Create laravel.yml
- Delete .github/workflows/main.yml
- Create main.yml

### 2024-04-20

- icon overlapped the buttons below it
- ignore compiled html files during development (unwanted hot reloading)
- refactor compilation handlers to reuse between build and share
- add check for layout initialization
- differentiate between compiler errors and linker errors
- relax filtering to include lines that contain "undefined symbol"
- fix issue #2
- formatting
- add indication of too fucking big
- prevent rapid-fire compiling
- SetupLayout if terms were already agreed to
- first launch TOS
- list stylings
- incate building, and error
- ya ya ya
- flesh out the share dialog
- tweak dialog styles
- remove dialog from home template
- break up styles
- fix app name
- remove extra block of stupid... and fix DEBIAN_FRONTEND environment variable
- remove permissions hack

### 2024-04-19

- fix storage permissions
- remove docker-compose.yml, used for a test.
- do caching in the entrypoint script instead
- remove comment

### 2024-04-18

- wide open, trust all the proxy!
- add micro to container
- nsjail, working
- ignored files
- use emscripten 3.1.56 because 57 is currently broken

### 2024-04-17

- docker image
- add logging to the compiler/linker that leaves a file in the workspace
- make caching optional
- filter compiler output
- add nsjail as option for compiling
- add trusted proxy, for test.pgetinker.com
- update README.md
- i don't know why you say goodbye, i say hello
- don't cry for me Argentina

### 2024-04-10

- clean up the cache builder
- use /opt/emsdk
- adjust paths to match the new layout
- consolidate server and client libraries
- move emscripten_shell -> etc
- move templates -> views
- update ignored files
- fix shareURL response

### 2024-04-06

- API change, call it shareURL instead of slug
- share urls need to work in development AND production

### 2024-04-05

- lots of stuff.. mainly implementing sharing... wip
- moved api handling to server.js, lots of reorganizing
- update watched files in dev mode
- add dialog styling
- disable sequelize logging
- use sequelize and sqlite3, update dev script to watch more files
- get code by slug
- post /api/code - compiles code, on successful compile, adds code to database and returns the sharable slug
- set up and use sequelize with sqlite3
- Generate a randomized 11 digit base64 slug

### 2024-04-04

- Console to use monospace font.. You're welcome, Tech
- add toggle the console functionality
- fix up the console and provide a means to toggle it's display
- add html download function
- handle theme changing in the player frame
- fix naming inconsistency
- formatting and integrate with themes
- alert that share is not implemented
- load dotenv config
- add status bar to editor panel
- load and keep default code in memory
- placeholder implementation for the Share api
- a whole lot.... basically reorganized UI handlers, added share.. and did away with some stupidity
- separate compile function from the api handler

### 2024-04-03

- close markers when getting default code
- player template
- update example environment variables
- marker stuff
- consolidate ui elements/click event callbacks
- switch hard-coded/embedded html to dynamically loaded template files
- add morgan for logging of http requests
- fix absolute include error message to match other emscripten error reporting
- remove cache volume in docker scripts
- switch emscripten version in docker builder

### 2024-04-02

- public assets
- vite first working rough draft
- update npm package files
- update ignored files
- removed -O2 optimization from build scripts

### 2024-04-01

- delete config.js
- delete public_html

### 2024-03-27

- use emscripten via submodule instead
- add emscripten as submodule
- remove console and info panel styles..
- console refinement
- show console output
- refine markers
- mark editor when errors
- filter output and use non-deprecated fs functions
- reorganize some code
- something in the default player panel
- remove temporary directory when finished
- use dotenv for configuration
- use build scripts instead of hard coded build commands
- dockerized build scripts
- local build scripts
- add custom docker image to cache building script

### 2024-03-26

- compile from button click
- add script to clean cache
- strip the player shell down to bare
- wip app js and css
- the page.. the app
- blank player for frame
- use custom emscripten shell
- add mktemp
- build inside node, no script calling
- indicate which unit is being built

### 2023-05-01

- Initial commit





