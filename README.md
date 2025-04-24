# PGEtinker
[![UnitTests](https://github.com/PGEtinker/pgetinker/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/PGEtinker/pgetinker/actions/workflows/unit-tests.yml)

Build an olcPixelGameEngine application interactively within your web browser!

Visit https://pgetinker.com

## System Requirements
* AMD Ryzen 5 3600X Processor (2 cores assigned).
* Ubuntu 22.04 or later.
* 4GB RAM
* 40GB Storage
* Docker Engine with compose and buildx plugins.
* A **STABLE** Internet Connection.

**Note:** PGEtinker may run on something less powerful or with
a different configuration than the requirements listed above.
The requirements are simply based upon the spec used on
PGEtinker's CI/CD runner.

## An Important Note About Root
PGEtinker and all of it's scripts expect that the directories
and files are owned by a normal user. The docker images that
are built by the scripts bake-in the permissions of a normal
user. Basically, trying to run it as `root` will permissions
problems. 

Remember with great power comes... I don't know, a big friggen
headache! Just, don't try to run this as `root`. Okay?

## Install PGEtinker

```bash
normal@host:~$ git clone https://github.com/PGEtinker/pgetinker
normal@host:~$ cd pgetinker
normal@host:~/pgetinker$ scripts/pgetinker initialize
```

### Update PGEtinker
```bash
normal@host:~/pgetinker$ scripts/pgetinker update
```

## Usage

### Bring up the PGEtinker services
```bash
normal@host:~/pgetinker$ scripts/pgetinker up -d
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
normal@host:~/pgetinker$ scripts/pgetinker npm run dev
```
**Note:** The frontend needs to be running at the same
time as the rest of the services.

## Development

A few things of note. PGEtinker is largely a Laravel project,
with a few other services which work together to make it all
happen. The local deployment is modified version of Laravel Sail,
which provides utilities to aid in development. This project has
alot of moving parts and this is the first time (3-27-2024) it's
been in a state where people could reasonably run it on their own
machines. I don't even know where to begin to give guidance on the
subject of contribution so if you got questions, send them my way!


## Known issues with the repo

* APP_URL must not contain a port. I spent alot of time trying to
get all of the services to work when APP_URL contains a port and
I have yet to figure out a sane way to handle that. For now, APP_URL
must have a proper address, with no port. Eg: http://localhost.
However, it does happily work behind a reverse proxy if APP_URL is,
for eg: https://test.pgetinker.com. This obviously requires a TLS
terminating reverse proxy configuration which is beyond the scope
of this manual.
