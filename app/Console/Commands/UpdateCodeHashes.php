<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;
use App\Models\Code;
use PGEtinker\Compiler;

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
        $compiler = new Compiler();
        $codes = Code::all();
        
        if(count($codes) == 0)
        {
            echo "Finished. Nothing to do.\n";
            return;
        }
        
        foreach($codes as $code)
        {
            $options = $compiler->getOptions();
            ksort($options);

            $libraries = $code->library_versions;
            ksort($libraries);

            $code->options = $options;
            $code->hash = hash("sha256", hashCode($code->code) . json_encode($libraries) . json_encode($options));
            $code->save();
        }

    }        
}
