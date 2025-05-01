<?php

namespace App\Console\Commands;

use App\Http\Controllers\CodeController;
use App\Models\Code;
use Illuminate\Console\Command;



class FindBrokenShares extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'app:find-broken-shares';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Tries to compile each share and provides a list of the slugs that failed.';

    /**
     * Execute the console command.
     */
    public function handle()
    {
        $codes = Code::all();
        $brokenSlugs = [];
        
        $controller = new CodeController();

        foreach($codes as $code)
        {
            $result = $controller->compileCode($code->code, $code->library_versions);
            if(!isset($result["html"]))
            {
                $brokenSlugs[] = $code->slug;
            }
        }

        if(count($brokenSlugs) > 0)
        {
            echo "\n";
            foreach($brokenSlugs as $slug)
            {
                echo "Slug: $slug... https://pgetinker.com/s/$slug\n";
            }
            echo "\n";
        }
    }
}
