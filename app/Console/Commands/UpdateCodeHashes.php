<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;
use App\Models\Code;
use function PGEtinker\Utils\hashCode;

class UpdateCodeHashes extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'app:update-code-hashes';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Code hash calculations have changed. Update them.';

    /**
     * Execute the console command.
     */
    public function handle()
    {
        $codes = Code::all();
        
        if(count($codes) == 0)
        {
            echo "Finished. Nothing to do.\n";
            return;
        }
        
        foreach($codes as $code)
        {
            $libraries = $code->library_versions;
            ksort($libraries);

            $code->hash = hash("sha256", hashCode($code->code) . json_encode($libraries));
            $code->save();
        }

    }        
}
