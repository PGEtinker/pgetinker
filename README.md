# PGEtinker
[![UnitTests](https://github.com/PGEtinker/pgetinker/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/PGEtinker/pgetinker/actions/workflows/unit-tests.yml)

Build an olcPixelGameEngine application interactively within your web browsesr!

## Requirements
* Linux Dev Environment.
* Docker Engine with compose and buildx plugins.

## Installation

```bash
git clone https://github.com/PGEtinker/pgetinker

cd pgetinker

scripts/pgetinker initialize
```

## Usage

### Bring up the PGEtinker services
```bash
scripts/pgetinker up -d
```
PGEtinker should now be available at: http://localhost

### Bring up the frontend dev server
```bash
scripts/pgetinker npm run dev
```
**Note:** The frontend needs to be running at the same time as the rest of the services. 

## Development

A few things of note. PGEtinker is largely a Laravel project, with a few other services which work together to make it all happen. The local deployment is modified version of Laravel Sail, which provides utilities to aid in development. This project has alot of moving parts and this is the first time (3-27-2024) it's been in a state where people could reasonably run it on their own machines. I don't even know where to begin to give guidance on the subject of contribution so if you got questions, send them my way!
