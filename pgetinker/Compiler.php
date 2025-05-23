<?php

namespace PGEtinker;

use Exception;
use Illuminate\Http\Client\PendingRequest;
use Illuminate\Support\Facades\Log;
use Illuminate\Support\Facades\Process;
use Illuminate\Support\Facades\Redis;
use Illuminate\Support\Facades\Storage;
use Monolog\Formatter\LineFormatter;
use Monolog\Handler\StreamHandler;
use Monolog\Logger;
use stdClass;

class Compiler
{
    private $code = [];
    
    private $compilerCommand = [];

    private $compilerExitCode;

    private $environmentVariables = [];
    
    private $errors = [];

    private $foundGeometryHeader = false;

    private $html = "";

    private $implementationMacros = [];

    private $linkerCommand = [];

    private $libraryDirectories = [];
    
    private $libraryMap = [];
    
    private $libraryVersions = [];

    private $linkerExitCode;

    private $linkerInputFiles = [];

    private $logger = null;

    private $output = [];

    private $workingDirectory = "";

    public function __construct()
    {
        $this->logger = new Logger("compiler");
        return $this;
    }
    
    public function setLibraryVersions($libraries)
    {
        $this->libraryVersions = $libraries;
    }

    public function getLibraryVersions()
    {
        return $this->libraryVersions;
    }

    public function serialize()
    {
        $object = new stdClass();

        $object->code = $this->code;
        $object->compilerCommand = $this->compilerCommand;
        $object->compilerExitCode = $this->compilerExitCode;
        $object->environmentVariables = $this->environmentVariables;
        $object->errors = $this->errors;
        $object->html = $this->html;
        $object->libraryVersions = $this->libraryVersions;
        $object->linkerCommand = $this->linkerCommand;
        $object->linkerExitCode = $this->linkerExitCode;
        $object->linkerInputFiles = $this->linkerInputFiles;
        $object->output = $this->output;

        return json_encode($object, JSON_PRETTY_PRINT);
    }

    public function deserialize(string $json)
    {
        $object = json_decode($json, false);
        
        $this->code = $object->code;
        $this->compilerCommand = $object->compilerCommand;
        $this->compilerExitCode = $object->compilerExitCode;
        $this->environmentVariables = $object->environmentVariables;
        $this->errors = $object->errors;
        $this->html = $object->html;
        $this->libraryVersions = $object->libraryVersions;
        $this->linkerCommand = $object->linkerCommand;
        $this->linkerExitCode = $object->linkerExitCode;
        $this->linkerInputFiles = $object->linkerInputFiles;
        $this->logger = null;
        $this->output = $object->output;
        $this->workingDirectory = "";
    }

    public function getCode()
    {
        return implode("\n", $this->code);
    }

    public function setCode(string $code)
    {
        $this->code = explode("\n", $code);
        return $this;
    }

    public function setWorkingDirectory(string $workingDirectory)
    {
        $this->workingDirectory = $workingDirectory;
        return $this;
    }

    public function getOutput($raw = false)
    {
        if($raw)
            return implode("\n", $this->output);
        
        $filtered = [];
        
        foreach($this->output as $text)
        {
            // skip cache messages
            if(strpos($text, "cache") === 0)
                continue; 

            $filtered[] = str_replace("/opt/emsdk/upstream/emscripten/cache/sysroot", "/***", $text);
        }

        return implode("\n", $filtered);
    }
    
    public function getErrorOutput($raw = false)
    {
        if($raw)
            return implode("\n", $this->errors);
        
        $filtered = [];
        
        foreach($this->errors as $text)
        {
            // skip cache messages
            if(strpos($text, "cache") === 0)
                continue; 

            $filtered[] = str_replace("/opt/emsdk/upstream/emscripten/cache/sysroot", "/***", $text);
        }

        return implode("\n", $filtered);
    }

    public function getHtml()
    {
        return $this->html;
    }

    public function getStatus()
    {
        return ($this->compilerExitCode == 0 && $this->linkerExitCode == 0) ?
            200 : 400;
    }

    private function processCodeAbsoluteOrRelativePaths($index)
    {
        // filter include macros with an absolute or relative path, naughty naughty
        preg_match(
            '/^\s*#\s*i(nclude|mport)(_next)?\s+["<]((\.{1,2}|\/)[^">]*)[">]/',
            $this->code[$index],
            $match,
            PREG_OFFSET_CAPTURE,
            0
        );

        if(count($match) > 0)
        {
            $this->errors[] = "/pgetinker.cpp:" . $index + 1 . ":1: error: absolute and relative includes are not allowed.";
            $this->logger->info("found absolute or relative path at line " . $index + 1);
            return true;
        }

        return false;
    }
    
    private function processCodeDetectGeometryUtility($index)
    {
        preg_match(
            '/^\s*#\s*i(nclude|mport)(_next)?\s+["<](.*)olcUTIL_Geometry2D.h[">]/',
            $this->code[$index],
            $match,
            PREG_OFFSET_CAPTURE,
            0
        );

        if(count($match) > 0)
        {
            $this->foundGeometryHeader = true;
            return true;
        }
    }

    private function processCodeDetectImplementationMacros($index)
    {
        // broad phase, don't process if this line doesn't at least contain the word "define"
        if(!str_contains($this->code[$index], "define"))
            return false;

        $foundImplementationMacro = false;
        
        foreach($this->libraryMap as $macro => $objectFileName)
        {
            preg_match(
                '/(.*)\s*#\s*define?\s+' . $macro . '(.*)/',
                $this->code[$index],
                $match,
                PREG_OFFSET_CAPTURE,
                0
            );
            
            // no match, this time
            if(count($match) == 0)
                continue;

            if(!empty(trim($match[1][0])) || !empty(trim($match[2][0])))
                continue;
            
            $this->implementationMacros[] = [
                "macro" => $macro,
                "lineIndex" => $index
            ];
            
            $foundImplementationMacro = true;
            break;
        }

        if($foundImplementationMacro)
            return true;
        
        return false;
    }

    private function processCodeRemoteInclude($index)
    {
        preg_match(
            '/^\s*#\s*i(nclude|mport)(_next)?\s+["<](https?:\/\/(.*)[^">]*)[">]/',
            $this->code[$index],
            $match,
            PREG_OFFSET_CAPTURE,
            0
        );

        if(count($match) > 0)
        {
            $this->logger->info("found a potential url for remote include");
            
            $potentialUrl = $match[3][0];
            $potentialFilename = basename($match[3][0]);
            $hashedUrl = hash("sha256", $potentialUrl);

            if(env("COMPILER_REMOTE_INCLUDE_CACHING", false))
            {
                try
                {
                    $remoteIncludeCache = Redis::get("remote_include_{$hashedUrl}");
                    
                    // if we have a cached version of the url's contents, don't pull it
                    if(isset($remoteIncludeCache))
                    {
                        $this->logger->info("remote include cache hit");
                        $remoteIncludeCache = json_decode($remoteIncludeCache, false);
                        
                        Redis::expire("remote_include_{$hashedUrl}", env("REDIS_TTL", 60));
                        
                        // just because it's cached, doesn't mean you get to compile faster!
                        usleep(floatval($remoteIncludeCache->time) * 1000000);
                        
                        file_put_contents(
                            "{$this->workingDirectory}/{$potentialFilename}",
                            $remoteIncludeCache->content
                        );
                        
                        $this->code[$index] = '#include "' . $potentialFilename .'"';
                        return true;
                    }
                }
                catch(Exception $e)
                {
                    Log::emergency("Remote Include Cache Enabled, But Redis Failed");
                }
            }
            
            $this->logger->info("remote include cache miss");
            
            try
            {
                $request = new PendingRequest();
                $request->timeout(3);
                $response = $request->head($potentialUrl);
            }
            catch(Exception $e)
            {
                $this->errors[] = "/pgetinker.cpp:" . $index + 1 . ":1: error: failed to retrieve {$potentialUrl}";
                $this->logger->info("failed to include remote file: {$potentialUrl} at line: " . $index + 1, [ "message" => $e->getMessage()]);
                return true;
            }
            
            if(
                !($response->status() >= 200 && $response->status() < 400) ||
                !str_contains($response->header("Content-Type"), "text/plain")
            )
            {
                $this->errors[] = "/pgetinker.cpp:" . $index + 1 . ":1: error: failed to retrieve {$potentialUrl}";
                $this->logger->info("failed to include remote file: {$potentialUrl} at line: " . $index + 1);
                return true;                    
            }

            if(intval($response->header("Content-Length")) > 1048576)
            {
                $this->errors[] = "/pgetinker.cpp:" . $index + 1 . ":1: error: exceeds 1MB maximum file size";
                $this->logger->info("remote file: {$potentialUrl} exceeds 1MB file size limitation");
                return true;
            }

            $this->logger->info("retrieving the body content");

            try
            {
                $requestStartTime = microtime(true);
                
                $request = new PendingRequest();
                $request->timeout(5);
                
                $response = $request->get($potentialUrl);
                
                $requestDuration = microtime(true) - $requestStartTime;
            }
            catch(Exception $e)
            {
                $this->errors[] = "/pgetinker.cpp:" . $index + 1 . ":1: error: failed to retrieve {$potentialUrl}";
                $this->logger->info("failed to include remote file: {$potentialUrl} at line: " . $index + 1);
                return true;
            }                
            
            // check included source for bad things
            preg_match_all(
                '/\s*#\s*i(nclude|mport)(_next)?\s+["<]((\.{1,2}|\/)[^">]*)[">]/m',
                $response->body(),
                $match,
                PREG_SET_ORDER,
                0
            );
            
            if(count($match) > 0)
            {
                $this->errors[] = "/pgetinker.cpp:" . $index + 1 . ":1: error: found absolute or relative paths in remote file: {$potentialUrl}";
                $this->logger->info("found absolute or relative paths in remote file: {$potentialUrl}");
                return true;
            }
            
            $this->logger->info("writing remote file to: {$this->workingDirectory}/{$potentialFilename}");
            file_put_contents(
                "{$this->workingDirectory}/{$potentialFilename}",
                $response->body()
            );
            
            if(env("COMPILER_REMOTE_INCLUDE_CACHING", false))
            {
                $this->logger->info("caching remotely included source file: $potentialFilename");

                $remoteIncludeCache = new stdClass();
                
                $remoteIncludeCache->time = $requestDuration;
                $remoteIncludeCache->content = $response->body();
                
                try
                {
                    Redis::setex("remote_include_{$hashedUrl}", env("REDI_TTL", 60), json_encode($remoteIncludeCache, JSON_PRETTY_PRINT));
                }
                catch(Exception $e)
                {
                    Log::emergency("Remote Inlucde Cache Enabled, But Redis Failed");
                }
            }

            $this->code[$index] = '#include "' . $potentialFilename .'"';
            return true;
        }
    }
    
    public function processCode()
    {
        $this->logger->info("begin processing code");
        
        $baseLibraryDirectory = env("PGETINKER_LIBS_DIRECTORY", "/opt/libs");

        if(!file_exists($baseLibraryDirectory . "/manifest.json"))
        {
            $this->errors[] = "library manifest doesn't exist";
            return false;
        }

        $temp = json_decode(file_get_contents($baseLibraryDirectory . "/manifest.json"), true);
        $this->libraryMap = $temp["macroToObject"];
        unset($temp);

        $startTime = microtime(true);

        for($i = 0; $i < count($this->code); $i++)
        {
            $endTime = microtime(true);
            $duration = $endTime - $startTime;

            if($duration > intval(env("COMPILER_CODE_PROCESSING_TIMEOUT", 5)))
            {
                $this->errors[] = "/pgetinker.cpp:" . $i . ":1: error: took too long to process your code, stopped here";
                return false;
            }
            
            if($this->processCodeDetectGeometryUtility($i))
                continue;
            
            if($this->processCodeAbsoluteOrRelativePaths($i))
                continue;

            if($this->processCodeDetectImplementationMacros($i))
                continue;

            if($this->processCodeRemoteInclude($i))
                continue;
        }


        if(count($this->implementationMacros) != 0)
        {
            foreach($this->implementationMacros as $implementation)
            {
                if($implementation["macro"] == "OLC_PGE_APPLICATION")
                {
                    if($this->foundGeometryHeader)
                    {
                        $this->linkerInputFiles[] = "olcPixelGameEngine/olcPixelGameEngine_withGeometry.o";
                        $this->code[$implementation["lineIndex"]] = "";
                        continue;
                    }
                }
                
                $this->linkerInputFiles[] = $this->libraryMap[$implementation["macro"]];
                $this->code[$implementation["lineIndex"]] = "";
                continue;
            }
        }
        
        $this->logger->info("finished processing code");
        
        return (count($this->errors) == 0);
    }
    
    private function prepareEnvironment()
    {
        $compilerEnvironment = env("COMPILER_ENVIRONMENT", "local");
        
        $baseLibraryDirectory = env("PGETINKER_LIBS_DIRECTORY", "/opt/libs");
        
        $libraries = $this->libraryVersions;
        $baseLibraryDirectory .= "/olcPixelGameEngine/" . $libraries["olcPixelGameEngine"];
        unset($libraries["olcPixelGameEngine"]);
        $this->libraryDirectories["olcPixelGameEngine"] = $baseLibraryDirectory . "/olcPixelGameEngine";

        foreach($libraries as $library => $version)
        {
            $this->libraryDirectories["{$library}"] = "{$baseLibraryDirectory}/{$library}/{$version}";
        }

        $this->logger->info("writing linesOfCode to {$this->workingDirectory}/pgetinker.cpp");
        file_put_contents(
            "{$this->workingDirectory}/pgetinker.cpp",
            implode("\n", $this->code)
        );

        if($compilerEnvironment === "local")
        {
            $this->logger->info("preparing compiler environment: {$compilerEnvironment}");
            
            $this->environmentVariables = array_merge($this->environmentVariables, [
                "EMSDK" => "/opt/emsdk",
                "EMSDK_NODE" => "/opt/emsdk/node/16.20.0_64bit/bin/node",
                "PATH" => "/bin:/usr/bin:/opt/emsdk:/opt/emsdk/upstream/emscripten",
            ]);
    
            foreach($this->libraryDirectories as $library => $directory)
            {
                symlink($directory, "{$this->workingDirectory}/{$library}");
            }

            symlink(base_path() . "/misc/emscripten_shell.html", "{$this->workingDirectory}/emscripten_shell.html");
        }

        if($compilerEnvironment === "nsjail")
        {
            $this->logger->info("preparing compiler environment: {$compilerEnvironment}");

            $nsJailCommand = [
                "nsjail",
                "--config",
                base_path() . env("COMPILER_NSJAIL_CFG", "/misc/nsjail-emscripten.cfg"),
                "-B",
                "{$this->workingDirectory}:/workspace",
                "-R",
                base_path() . "/misc/emscripten_shell.html:/workspace/emscripten_shell.html",
            ];

            foreach($this->libraryDirectories as $library => $directory)
            {
                $nsJailCommand[] = "-R";
                $nsJailCommand[] = "{$directory}:/workspace/{$library}";
            }

            if(config("app.env") === "production")
            {
                // run nsjail in "really quiet" mode during production.
                $nsJailCommand[] = "-Q";
            }
            
            $nsJailCommand[] = "--";

            $this->compilerCommand = $nsJailCommand;
            $this->linkerCommand   = $nsJailCommand;
        }

        $this->logger->info("preparing compiler command");
        $this->compilerCommand = array_merge($this->compilerCommand, [
            "/opt/emsdk/upstream/emscripten/em++",
            "-c",
            "-O1",
            "-I./miniaudio",
            "-I./olcPGEX_Gamepad",
            "-I./olcPGEX_MiniAudio",
            "-I./olcPixelGameEngine",
            "-I./olcPixelGameEngine/extensions",
            "-I./olcPixelGameEngine/utilities",
            "-I./olcSoundWaveEngine",
            "pgetinker.cpp",
            "-o",
            "pgetinker.o",
            "-std=c++20",
        ]);
        $this->logger->info("Compiler command:\n\n" . implode("\n", $this->compilerCommand) . "\n");

        $this->logger->info("preparing linker command");
        $this->linkerCommand = array_merge($this->linkerCommand, [
            "/opt/emsdk/upstream/emscripten/em++",
            "pgetinker.o",
            ...$this->linkerInputFiles,
            "-o",
            "pgetinker.html",
            "--shell-file",
            "./emscripten_shell.html",
            "-sASYNCIFY",
            "-sALLOW_MEMORY_GROWTH=1",
            "-sSTACK_SIZE=131072",
            "-sMAX_WEBGL_VERSION=2",
            "-sMIN_WEBGL_VERSION=2",
            "-sUSE_LIBPNG=1",
            "-sUSE_SDL_MIXER=2",
            "-sLLD_REPORT_UNDEFINED",
            "-sSINGLE_FILE",
            "-std=c++20",
        ]);
        $this->logger->info("Linker command:\n\n" . implode("\n", $this->linkerCommand) . "\n");

        
        return true;
    }

    private function compile()
    {
        $this->logger->info("invoking the compiler");
        
        $didTheThingSuccessfully = false;
        
        try
        {
            $compilerProcessResult = Process::env($this->environmentVariables)
                ->path($this->workingDirectory)
                ->timeout(intval(env("COMPILER_TIMEOUT", 10)))
                ->command($this->compilerCommand)->run();
            
            $this->logger->info("compiler exited with code: " . $compilerProcessResult->exitCode());
            
            $this->compilerExitCode = $compilerProcessResult->exitCode();

            $didTheThingSuccessfully = ($this->compilerExitCode == 0);
            
            $this->output = array_merge(
                $this->output,
                explode("\n", $compilerProcessResult->output())
            );

            $this->errors = array_merge(
                $this->errors,
                explode("\n", $compilerProcessResult->errorOutput())
            );

            if($this->compilerExitCode == 137)
            {
                $this->errors[] = "Compiler Killed (SIGTERM)";
            }
        }
        catch(Exception $e)
        {
            $this->errors[] = "compiler timed out. your code is either broken or there's too much of it!";
            $didTheThingSuccessfully = false;
        }

        return $didTheThingSuccessfully;
    }

    private function link()
    {
        $this->logger->info("invoking the linker");
        
        $didTheThingSuccessfully = false;
        
        try
        {
            $linkerProcessResult = Process::env($this->environmentVariables)
                ->path($this->workingDirectory)
                ->timeout(intval(env("COMPILER_TIMEOUT", 10)))
                ->command($this->linkerCommand)->run();
            
            $this->logger->info("compiler exited with code: " . $linkerProcessResult->exitCode());

            $this->linkerExitCode = $linkerProcessResult->exitCode();
            
            $didTheThingSuccessfully = ($this->linkerExitCode == 0);
            
            $this->output = array_merge(
                $this->output,
                explode("\n", $linkerProcessResult->output())
            );

            $this->errors = array_merge(
                $this->errors,
                explode("\n", $linkerProcessResult->errorOutput())
            );

            if($this->compilerExitCode == 137)
            {
                $this->errors[] = "Linker Killed (SIGTERM)";
            }
        }
        catch(Exception $e)
        {
            $this->errors[] = "linker timed out. your code is either broken or there's too much of it!";
            $didTheThingSuccessfully = false;
        }

        return $didTheThingSuccessfully;
    }
    
    private function cleanUp()
    {
        $this->logger->info("OUTPUT:\n" . $this->getOutput());
        $this->logger->info("ERROR:\n" . $this->getErrorOutput());
        $this->logger->info("LIBRARIES:\n" . implode("\n", $this->linkerInputFiles));
        
        Log::info("Compile: finished disgracefully");
    }

    public function build()
    {
        if(!file_exists($this->workingDirectory) || !is_dir($this->workingDirectory))
            throw new Exception("Working Directory Inaccessible. Did you set one?");

        $logHandler = new StreamHandler("{$this->workingDirectory}/compiler.log");
        $logHandler->setFormatter(new LineFormatter(null, null, true, true));
        
        $this->logger->setHandlers([$logHandler]);

        if(empty($this->libraryVersions))
        {
            $manifestPath = env("PGETINKER_LIBS_DIRECTORY", "/opt/libs") . "/manifest.json";
            
            if(!file_exists($manifestPath))
                throw new Exception("Library manifest does not exist!");
            
            $manifest = json_decode(file_get_contents($manifestPath), true);
            $this->libraryVersions = $manifest["latest"];
            $this->logger->info("Libraries not set, using latest as default!");
        }
        

        if(!$this->processCode())
        {
            $this->cleanUp();
            return false;
        }

        if(!$this->prepareEnvironment())
        {
            return false;
        }

        if(!$this->compile())
        {
            $this->cleanUp();
            return false;
        }

        if(!$this->link())
        {
            $this->cleanUp();
            return false;
        }

        if(file_exists("{$this->workingDirectory}/pgetinker.html"))
        {
            $this->html = file_get_contents("{$this->workingDirectory}/pgetinker.html");
            
            // convert workingDirectory to laravel disk path
            $prefix = dirname($this->workingDirectory);
            $this->workingDirectory = str_replace("{$prefix}/", "", $this->workingDirectory);
            
            Storage::disk("local")->deleteDirectory($this->workingDirectory);

            $this->logger->info("OUTPUT:\n" . $this->getOutput());
            $this->logger->info("ERROR:\n" . $this->getErrorOutput());
            $this->logger->info("LIBRARIES:\n" . implode("\n", $this->linkerInputFiles));

            Log::info("Compile: finished successfully");
            $this->output[] = "Compiled Successfully";
            return true;
        }
        
        $this->cleanUp();
        return false;
    }

    public function healthCheck()
    {
        $this->setWorkingDirectory("/tmp");

        try
        {
            $compilerProcessResult = Process::env($this->environmentVariables)
                ->path($this->workingDirectory)
                ->timeout(intval(env("COMPILER_TIMEOUT", 10)))
                ->command([
                    "nsjail",
                    "--config",
                    base_path() . env("COMPILER_NSJAIL_CFG", "/misc/nsjail-emscripten.cfg"),
                    "-B",
                    "{$this->workingDirectory}:/workspace",
                    "--",
                    "/opt/emsdk/upstream/emscripten/em++",
                    "-v",
                ])
                ->run();
            
            $this->compilerExitCode = $compilerProcessResult->exitCode();
            
            $didTheThingSuccessfully = ($this->compilerExitCode == 0);
            
            $this->output = array_merge(
                $this->output,
                explode("\n", $compilerProcessResult->output())
            );

            $this->errors = array_merge(
                $this->errors,
                explode("\n", $compilerProcessResult->errorOutput())
            );

            if($this->compilerExitCode == 137)
            {
                $this->errors[] = "Compiler Killed (SIGTERM)";
            }
        }
        catch(Exception $e)
        {
            $this->errors[] = "compiler timed out on health check";
            $didTheThingSuccessfully = false;
        }

        return $didTheThingSuccessfully;
    }
}
