<?php

namespace App\Providers;

use Illuminate\Support\Facades\Config;
use Illuminate\Support\Facades\Log;
use Illuminate\Support\Facades\Request;
use Illuminate\Support\Facades\URL;
use Illuminate\Support\ServiceProvider;

class AppServiceProvider extends ServiceProvider
{
    /**
     * Register any application services.
     */
    public function register(): void
    {
        //
    }

    /**
     * Bootstrap any application services.
     */
    public function boot(): void
    {
        //
        URL::forceRootUrl(Request::getSchemeAndHttpHost());
        
        Config::set('app.version', function ()
        {
            return cache()->remember('app_version', now()->addSeconds(10), function()
            {
                Log::info("cache miss on app.version");
                try {
                    $branch = trim(exec('git rev-parse --abbrev-ref HEAD 2>/dev/null', $output, $returnCode));
                    $commit = trim(exec('git rev-parse --short HEAD 2>/dev/null', $output, $returnCode));
                    return $returnCode === 0 ? "{$branch}@{$commit}" : 'unknown';
                } catch (\Exception $e) {
                    return 'unknown';
                }
            });
        });
    }
}
