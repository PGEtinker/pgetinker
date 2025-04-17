<?php

namespace App\Console\Commands;

use App\Models\Code;
use Exception;
use Illuminate\Console\Command;
use Illuminate\Support\Facades\DB;

class UpdateDatabase extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'app:update-database';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Update the database if a migration requires new data.';

    /**
     * Execute the console command.
     */
    public function handle()
    {

        $manifestPath = "/opt/libs/manifest.json";
            
        if(!file_exists($manifestPath))
            throw new Exception("Library manifest does not exist!");
        
        $manifest = json_decode(file_get_contents($manifestPath), true);
        $libraries = $manifest["latest"];

        $codes = Code::all();
        
        for($i = 0; $i < count($codes); $i++)
        {
            echo $codes[$i]->slug . "\n";
            $codes[$i]->library_versions = $libraries;
            $codes[$i]->save();
        }
    }
}
