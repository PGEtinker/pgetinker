#!/usr/bin/bash

# refresh the cache at startup
su -c "php artisan config:clear" -s /bin/bash www-data
su -c "php artisan cache:clear" -s /bin/bash www-data
su -c "php artisan view:clear" -s /bin/bash www-data
su -c "php artisan route:clear" -s /bin/bash www-data

# the server
apache2-foreground
