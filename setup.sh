#!/usr/bin/bash
set -e

install_or_update_repo() {
    echo "Checking for: $1"
    if [ ! -d "$1" ]; then
        echo "Cloning $1"
        docker run --rm -it -v ./PGEtinker:/home/pgetinker/tmp pgetinker/devkit bash -c "git clone https://github.com/$1.git"
    else
        echo "$1 exists. Updating."
        docker run --rm -it -v "./$1":/home/pgetinker/tmp pgetinker/devkit bash -c "git pull"
    fi
    echo ""
}

command_exists() {
    if command -v "$1" &> /dev/null; then
        return 0
    else
        return 1
    fi
}

# Welcome message
echo "Welcome to the PGEtinker Devkit Setup Script"
echo "============================================"
echo

echo "Checking for supported CPU architecture (x86_64)."
if [ ! "x86_64" = "$(uname -m)" ]; then
    echo "Unsupported architecture detected. PGEtinker currently uses architecture"
    echo "dependent software. This means a x86_64 CPU is requires. Setup can not continue."
    echo ""
    exit 4
fi
echo "Found x86_64 CPU architecture."

echo "Checking for docker."
if ! command_exists "docker"; then
    echo "Docker is not installed. Please follow your distro's instructions to install it."
    echo "https://docs.docker.com/engine/install/"
    echo ""
    exit 1
fi
echo "Found docker."
echo ""

echo "Checking for docker compose."
if ! command_exists "docker-compose"; then
    if ! docker compose &> /dev/null; then
        echo "Docker Compose is not installed. Please follow your distro's instructions to install it."
        echo "https://docs.docker.com/engine/install/"
        echo ""
        exit 2
    fi
fi
echo "Found docker compose."
echo ""

echo "Checking for docker buildx."
if ! docker buildx &> /dev/null; then
    echo "Docker Buildx is not installed. Please follow your distro's instructions to install it."
    echo "https://docs.docker.com/engine/install/"
    echo ""
    exit 3
fi
echo "Found docker buildx."
echo ""

echo "Checking for devkit files."
if [ ! -f devkit/docker-entrypoint.sh ] \
    || [ ! -f devkit/Dockerfile ] \
    || [ ! -f devkit/php.ini ] \
    || [ ! -f devkit/supervisord.conf ]; then

    echo "Devkit files are missing!"
    echo "Please ensure that you are in the root of the"
    ehco "PGEtinker/pgetinker repo before proceeding!"
    exit 4
fi
echo "Found devkit files."

echo "This devkit relies on multiple repos that interact with"
echo "each other to provide the PGEtinker experience."
echo "By default, these repos are pulled by this script."
echo ""
echo "If you plan to develop PGEtinker, it is recommended"
echo "that you provide the paths to your forks in the .env"
echo "configuration."
echo ""
echo -n "Would you like this script to pull the main repos for you? (Y/n) "; read pull_repos

pull_repos=${pull_repos:-Y}


case $pull_repos in
    [Yy]*)
        echo "You chose to install the main repos!"
        echo ""

        mkdir -p PGEtinker
        
        install_or_update_repo "PGEtinker/app"
        install_or_update_repo "PGEtinker/language-server"
        install_or_update_repo "PGEtinker/libs"
        install_or_update_repo "PGEtinker/screenshot"
        install_or_update_repo "PGEtinker/wiki"
        
        echo "Repositories installed!"
        ;;
    [Nn]*)
        echo "You chose to skip installing the main repos!"
        echo ""
        ;;
    *)
        echo "You chose to skip installing the main repos!"
        echo ""
        ;;
esac

echo "Checking for .env"
if [ ! -f .env ]; then
    echo ".env not found. Creating."
    cp .env-example .env
else
    echo ".env found."
fi
echo ""

echo "To spin up PGEtinker locally, run: docker compose up -d"
echo "NOTE: if you have the standalone compose installed the command is docker-compose"
echo "Access it via web browser at http://localhost"
echo ""

exit 0
