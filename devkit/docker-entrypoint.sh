#!/usr/bin/bash
set -e

# Function to check Laravel app and dependencies
check_laravel_app() {
    local app_dir="/home/pgetinker/app"  # First argument is the app directory

    echo "#### BEGIN: checking for laravel app at $app_dir"
    
    # Check if directory exists
    if [ ! -d "$app_dir" ]; then
        echo "## Error: Directory '$app_dir' does not exist."
        return 1
    fi

    # Check for Laravel-specific files
    if [ ! -f "$app_dir/artisan" ] || [ ! -f "$app_dir/composer.json" ]; then
        echo "## Error: '$app_dir' does not contain PGEtinker/app."
        echo "## Did you clone the repo and set it's location in the .env file?"
        echo ""
        return 2
    fi

    echo "## Laravel application found in '$app_dir'"
    cd $app_dir

    # Check for vendor directory (Composer dependencies)
    if [ ! -d "$app_dir/vendor" ]; then
        echo "## Composer dependencies not installed. Attempting to install"
        
        composer install --no-interaction --quiet > /dev/null 2>&1
        
        if [ $? -eq 0 ]; then
            echo "## Composer dependencies installed."
        else
            echo "## Composer dependencies failed to install."
            return 3
        fi
    else
        echo "## Composer dependencies found."
    fi

    # Check for node_modules directory (NPM dependencies)
    if [ ! -d "$app_dir/node_modules" ]; then
        echo "## NPM dependencies not installed. Attempting to install"
        
        npm install --quiet > /dev/null 2>&1
        
        if [ $? -eq 0 ]; then
            echo "## NPM dependencies installed."
        else
            echo "## NPM dependencies failed to install."
            return 4
        fi
    else
        echo "## NPM dependencies found."
    fi

    # Check for database driver and do a fresh migration if needed.
    if [ "$DB_CONNECTION" = "" ] || [ "$DB_CONNECTION" = "sqlite3" ]; then
        echo "## Database connection not set, assuming sqlite3."

        if [ ! -f database/database.sqlite ]; then
            echo "## Database not found, creating."
            
            touch database/database.sqlite
            php artisan migrate --force --no-interaction
            
            if [ $? -eq 0 ]; then
                echo "## Database created."
            else
                echo "## Database failed to migrate."
                return 5
            fi
        else
            echo "## Database found."
        fi
    fi

    echo "#### END: checking for laravel app at $app_dir" && echo ""
    return 0
}

check_language_server() {
    local app_dir="/home/pgetinker/language-server"
    echo "#### BEGIN: checking for language server at $app_dir"
    
    # Check if directory exists
    if [ ! -d "$app_dir" ]; then
        echo "## Error: Directory '$app_dir' does not exist."
        return 1
    fi    
    
    # Check for language-server specific files
    if [ ! -f "$app_dir/server.ts" ] || [ ! -f "$app_dir/package.json" ]; then
        echo "## Error: '$app_dir' does not contain PGEtinker/language-server."
        echo "## Did you clone the repo and set it's location in the .env file?"
        echo ""
        return 2
    fi

    cd $app_dir

    # Check for node_modules directory (NPM dependencies)
    if [ ! -d "$app_dir/node_modules" ]; then
        echo "## NPM dependencies not installed. Attempting to install"
        
        npm install --quiet > /dev/null 2>&1
        
        if [ $? -eq 0 ]; then
            echo "## NPM dependencies installed."
        else
            echo "## NPM dependencies failed to install."
            return 4
        fi
    else
        echo "## NPM dependencies found."
    fi

    echo "#### END: checking for language server at $app_dir" && echo ""
    return 0
}

check_screenshot() {
    local app_dir="/home/pgetinker/screenshot"
    echo "#### BEGIN: checking for screenshot at $app_dir"
    
    # Check if directory exists
    if [ ! -d "$app_dir" ]; then
        echo "## Error: Directory '$app_dir' does not exist."
        return 1
    fi    

    # Check for screenshot specific files
    if [ ! -f "$app_dir/index.js" ] || [ ! -f "$app_dir/package.json" ]; then
        echo "## Error: '$app_dir' does not contain PGEtinker/screenshot."
        echo "## Did you clone the repo and set it's location in the .env file?"
        echo ""
        return 2
    fi

    cd $app_dir

    # Check for node_modules directory (NPM dependencies)
    if [ ! -d "$app_dir/node_modules" ]; then
        echo "## NPM dependencies not installed. Attempting to install"
        
        npm install --quiet > /dev/null 2>&1
        
        if [ $? -eq 0 ]; then
            echo "## NPM dependencies installed."
        else
            echo "## NPM dependencies failed to install."
            return 4
        fi
    else
        echo "## NPM dependencies found."
    fi

    echo "#### END: checking for screenshot at $app_dir" && echo ""
    return 0
}

check_libraries() {
    local app_dir="/opt/libs"
    echo "#### BEGIN: checking for pgetinker libraries at $app_dir"
    
    # Check if directory exists
    if [ ! -d "$app_dir" ]; then
        echo "## Error: Directory '$app_dir' does not exist."
        return 1
    fi    

    # Check for pgetinker library specific files
    if [ ! -f "$app_dir/manifest.json" ] || [ ! -f "$app_dir/Makefile" ]; then
        echo "## Error: '$app_dir' does not contain PGEtinker/libs."
        echo "## Did you clone the repo and set it's location in the .env file?"
        echo ""
        return 2
    fi

    echo "## PGEtinker libraries found in '$app_dir'"
    cd $app_dir
    
    # build the libraries, if needed.
    bash build-libs.sh > /dev/null 2>&1
    
    if [ $? -eq 0 ]; then
        echo "## PGEtinker libraries built."
    else
        echo "## PGEtinker libraries failed to build."
        return 3
    fi

    echo "#### END: checking for pgetinker libraries at $app_dir" && echo ""

    return 0
}

check_wiki() {
    local app_dir="/home/pgetinker/wiki"
    echo "#### BEGIN: checking for wiki at $app_dir"

    # Check if directory exists
    if [ ! -d "$app_dir" ]; then
        echo "## Error: Directory '$app_dir' does not exist."
        return 1
    fi

    # Check for wiki specific files
    if [ ! -f "$app_dir/hugo.toml" ]; then
        echo "## Error: '$app_dir' does not contain PGEtinker/wiki."
        echo "## Did you clone the repo and set it's location in the .env file?"
        echo ""
        return 2
    else
        echo "## wiki found."
    fi

    echo "#### END: checking for wiki at $app_dir" && echo ""

    return 0    
}

if [ ! $# -eq 0 ]; then
    cd /home/pgetinker/tmp
    exec "$@"
    exit 0
fi

FAILED=0

# Call the function with the provided directory
check_laravel_app || {
    FAILED=1
}

check_language_server || {
    FAILED=1
}

check_screenshot || {
    FAILED=1
}

check_libraries || {
    FAILED=1
}

check_wiki || {
    FAILED=1
}

if [ $FAILED -eq 0 ]; then
    echo "#### PGEtinker startup tests have passed. Starting services. ####"
    echo ""
    cd /home/pgetinker
    exec supervisord -c /etc/supervisor/conf.d/supervisord.conf
else
    echo "## One or more commands failed, see above."
    exit 1
fi
