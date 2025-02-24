#!/usr/bin/bash

echo -n "Checking for Emscripten ... "
if [ ! -e /opt/emsdk/README.md ] ; then
    echo "Not Found."
    exit 1
else
    echo "Found."
fi

echo -n "Checking for PGEtinker libraries ... "
if [ ! -e /opt/PGEtinker-libs/manifest.json ] ; then
    echo "Not Found."
    exit 1
else
    echo "Found."
    echo "Building, if needed"
    (cd /opt/PGEtinker-libs; bash build-libs.sh)
fi

echo -n "Check if using SQLITE driver ... "
DATABASE=$(su -c "php artisan app:identify-database" -s /bin/bash www-data)
if [ "$DATABASE" = "sqlite" ] ; then
    echo "yes."
    
    echo -n "Check for database file... "
    if [ ! -e /var/www/html/database/database.sqlite ] ; then
        echo "Not Found... Creating."
        touch /var/www/html/database/database.sqlite
        chown www-data:www-data /var/www/html/database/database.sqlite
        chmod 644 -R /var/www/html/database/database.sqlite
        
        echo "Installing database!"
        su -c "php artisan migrate:fresh" -s /bin/bash www-data
    else
        echo "Database file found!"
    fi
else
    echo "no."
fi

# refresh the cache at startup
su -c "php artisan config:clear" -s /bin/bash www-data
su -c "php artisan cache:clear" -s /bin/bash www-data
su -c "php artisan view:clear" -s /bin/bash www-data
su -c "php artisan route:clear" -s /bin/bash www-data

# the server
echo "Starting Apache."
apache2-foreground
