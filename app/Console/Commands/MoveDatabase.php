<?php

namespace App\Console\Commands;

use App\Models\Code;
use Exception;
use Illuminate\Console\Command;
use Illuminate\Support\Facades\DB;

class MoveDatabase extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'app:move-database';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Move data from supabase to the default database.';

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

        //
        $codes = DB::connection('supa')
            ->table('codes')
            ->orderBy('id', 'asc')
            ->get();
        
        for($i = 0; $i < count($codes); $i++)
        {
            echo $codes[$i]->slug . "\n";
            $code = new Code();
            $code->id         = $codes[$i]->id;
            $code->slug       = $codes[$i]->slug;
            $code->code       = $codes[$i]->code;
            $code->hash       = $codes[$i]->hash;
            $code->library_versions = $libraries;
            $code->view_count = $codes[$i]->view_count;
            $code->created_at = $codes[$i]->created_at;
            $code->save();
        }
        
        // Set the new auto-increment value (e.g., after the highest ID)
        $maxId = Code::max('id');
        $newAutoIncrement = $maxId + 1;
        echo "New Auto Increment: " . $newAutoIncrement . "\n\n";
        DB::query("ALTER TABLE codes AUTO_INCREMENT = " . $newAutoIncrement);
    }
}
