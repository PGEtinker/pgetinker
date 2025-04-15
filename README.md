# PGEtinker
[![UnitTests](https://github.com/PGEtinker/pgetinker/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/PGEtinker/pgetinker/actions/workflows/unit-tests.yml)

Build an olcPixelGameEngine application interactively within your web browsesr!

Visit https://pgetinker.com

## System Requirements
* AMD Ryzen 5 3600X Processor (2 cores assigned).
* 4GB RAM
* 40GB Storage
* Docker Engine with compose and buildx plugins.

**Note:** PGEtinker may run on something less powerful or with
a different configuration than the requirements listed above.
The requirements are simply based upon the spec used on
PGEtinker's CI/CD runner.

## Install PGEtinker

```bash
git clone https://github.com/PGEtinker/pgetinker

cd pgetinker

scripts/pgetinker initialize
```

### Update PGEtinker
```bash
scripts/pgetinker update
```

## Usage

### Bring up the PGEtinker services
```bash
scripts/pgetinker up -d
```

This will bring PGEtinker services up in their
default mode. In this state you can enjoy PGEtinker
as a general user by visiting: http://localhost, or
whatever address you configured ``APP_URL`` to be in
the ``.env`` configuration file. If you wish to make
changes to the frontend code, then you will need to
also start the frontend dev server, see below.

### Bring up the frontend dev server
```bash
scripts/pgetinker npm run dev
```
**Note:** The frontend needs to be running at the same time as the rest of the services. 

## Development

A few things of note. PGEtinker is largely a Laravel project, with a few other services which work together to make it all happen. The local deployment is modified version of Laravel Sail, which provides utilities to aid in development. This project has alot of moving parts and this is the first time (3-27-2024) it's been in a state where people could reasonably run it on their own machines. I don't even know where to begin to give guidance on the subject of contribution so if you got questions, send them my way!

## Known issues with the repo

* Sharing Screenshots haven't been added yet.
* The services can not be started on arbitrary ports, you get port 80 and that's it. However the reverse proxy service does forward the protocols properly so if it's behind a TLS terminator reverse proxy you can host it publicly on a HTTPS address.

