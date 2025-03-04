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
        echo "## Error: '$app_dir' does not appear to be a Laravel application."
        echo "## Missing artisan or composer.json files."
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

    # TODO: Check for database driver and do a fresh migration if needed.
    if [ "$DB_CONNECTION" = "" ]; then
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
        echo "## Error: '$app_dir' does not appear contain the pgetinker libraries."
        return 2
    fi

    echo "## PGEtinker libraries found in '$app_dir'"
    cd $app_dir

    bash build-libs.sh
    
    if [ $? -eq 0 ]; then
        echo "## PGEtinker libraries built."
    else
        echo "## PGEtinker libraries failed to build."
        return 3
    fi

    echo "#### END: checking for pgetinker libraries at $app_dir" && echo ""

    return 0
}

# hugo.toml


# Call the function with the provided directory
check_laravel_app || {
    echo "## Error: the laravel app failed catastrophically and can not start."
    exit 1
}

check_language_server || {
    echo "## Error: the language server failed catastrophically and can not start."
    exit 2
}

check_screenshot || {
    echo "## Error: the screenshot service failed catastrophically and can not start."
    exit 3
}

check_libraries || {
    echo "## Error: the pgetinker libraries could not be found or built."
    exit 4
}

check_wiki || {
    echo "## Error: the wiki failed catastrophically and can not start."
    exit 5
}

cd /home/pgetinker

exec supervisord -c /etc/supervisor/conf.d/supervisord.conf