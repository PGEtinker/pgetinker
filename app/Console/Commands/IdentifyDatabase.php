<?php

namespace App\Console\Commands;

use App\Models\Code;
use Illuminate\Console\Command;
use Illuminate\Support\Facades\DB;

class MoveDatabase extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'app:identify-database';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Determine the database driver. For use in container discovery.';

    /**
     * Execute the console command.
     */
    public function handle()
    {
        $connection = config('database.default');
        $driver = config("database.connections.{$connection}.driver");
        echo $driver . "\n";
        
    }
}
