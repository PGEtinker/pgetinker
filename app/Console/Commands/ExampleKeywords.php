<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;

function strip_comments_strings_directives($source) {
    // Remove preprocessor directives
    $source = preg_replace('/^\s*#.*$/m', '', $source);
    
    // Remove comments and string literals
    $pattern = '/\/\/.*?$|\/\*.*?\*\/|"(?:\\\\.|[^\\\\"])*"|\'(?:\\\\.|[^\\\\\'])*\'/ms';
    return preg_replace($pattern, '', $source);
}

function extract_identifiers($source) {
    // Strip comments, strings, and directives
    $source_clean = strip_comments_strings_directives($source);
    
    // C/C++ keywords to exclude
    $keywords = [
        'auto', 'break', 'case', 'char', 'const', 'continue', 'default', 'do',
        'double', 'else', 'enum', 'extern', 'float', 'for', 'goto', 'if', 'inline',
        'int', 'long', 'register', 'return', 'short', 'signed', 'sizeof', 'static',
        'struct', 'switch', 'typedef', 'union', 'unsigned', 'void', 'volatile',
        'while', 'class', 'public', 'private', 'protected', 'virtual', 'new', 'delete',
        'template', 'typename', 'namespace', 'using', 'true', 'false', 'nullptr'
    ];
    
    // Pattern for identifiers: alphanumeric with underscores
    $identifier_pattern = '/\b[a-zA-Z_][a-zA-Z0-9_]*\b/';
    
    // Extract all identifiers
    $identifiers = [];
    preg_match_all($identifier_pattern, $source_clean, $matches);
    
    foreach ($matches[0] as $ident) {
        // Exclude keywords, numbers, and common literals
        if (!in_array($ident, $keywords) && !ctype_digit($ident) &&
            !in_array($ident, ['true', 'false', 'NULL', 'nullptr'])) {
            $identifiers[] = $ident;
        }
    }
    
    // Remove duplicates and sort
    $identifiers = array_unique($identifiers);
    sort($identifiers);
    
    return $identifiers;
}

class ExampleKeywords extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'app:example-keywords';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Generates the keywords field for example search data from the example source files.';

    /**
     * Execute the console command.
     */
    public function handle()
    {
        $examples = json_decode(file_get_contents(base_path() . "/examples/examples.json"), true);
        
        foreach($examples as &$example)
        {
            $source = file_get_contents(base_path() . "/" . $example["codeFile"]);
            // Extract identifiers
            $example["keywords"] = implode(' ', extract_identifiers($source));
        }

        file_put_contents(base_path() . "/examples/examples.json", json_encode($examples, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES));
    }        
}
