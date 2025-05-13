<?php

namespace App\Http\Controllers;

use App\Models\Code;
use Exception;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Http;
use Illuminate\Support\Facades\Log;
use Illuminate\Support\Facades\Redis;
use Illuminate\Support\Facades\Request as FacadesRequest;
use Illuminate\Support\Facades\Storage;
use Illuminate\Support\Str;

use PGEtinker\Compiler;


use function PGEtinker\Utils\hashCode;

use function PGEtinker\Utils\takeScreenshotOfHtml;
use function PGEtinker\Utils\uploadFileToPit;

class CodeController extends Controller
{
    function Compile(Request $request)
    {
        $result = $this->compileCode($request->input("code", null), $request->input("libraries", null));
        unset($result["hash"]);
        return response($result, $result["statusCode"])->header("Content-Type", "application/json");
    }
    
    function GetShare(Request $request, string $slug)
    {
        $code = Code::where("slug", $slug)->first();
        if($code)
            return $code;

        return response(["message" => "not found"], 404, [
            "Content-Type" => "application/json"
        ]);
    }

    function Share(Request $request)
    {
        $code   = $request->input("code", null);
        $libraries = $request->input("libraries", null);
        $result = $this->compileCode($code, $libraries);
    
        // if failed to compile, bail
        if($result["statusCode"] !== 200)
        {
            unset($result["hash"]);
            return response($result, $result["statusCode"])->header("Content-Type", "application/json");
        }
    
        // check if we've already tried to share this code
        $share = Code::where("hash", $result["hash"])->first();
        if($share != null)
        {
            $result["shareURL"] = FacadesRequest::getSchemeAndHttpHost() . "/s/" . $share->slug;

            if(empty($share->thumb_url))
            {
                Log::info("no thumbnail, try to upload it");
                $share->thumb_url = uploadFileToPit($share->slug . ".png", takeScreenshotOfHtml($result["html"]));
                $share->save();
            }

            $result["shareThumbURL"] = $share->thumb_url;
            unset($result["hash"]);
            return response($result, $result["statusCode"])->header("Content-Type", "application/json");
        }
        
        // if we're here, we got a bonafide, unique, and working code to share with the world
        
        // try to make a unique slug
        $tryAgain = true;
        $slug = "";
        do
        {
            // thanks Bixxy and Ciarán for the feedback in #help-each-other
            $slug = str_replace(['+','/','='], ['-','',''], substr(base64_encode(sha1(microtime(),true)), 0, 11));

            if(Code::where("slug", $slug)->first() == null);
            {
                $tryAgain = false;
            }
        } while($tryAgain);
        
        $share = new Code();
        $share->code = $code;
        $share->hash = $result["hash"];
        $share->slug = $slug;
        $share->thumb_url = uploadFileToPit($share->slug . ".png", takeScreenshotOfHtml($result["html"]));
        $share->library_versions = $libraries;
        
        if($share->save())
        {
            $result["shareURL"] = FacadesRequest::getSchemeAndHttpHost() . "/s/" . $slug;
            $result["shareThumbURL"] = $share->thumb_url;
            unset($result["hash"]);

            return response($result, $result["statusCode"])->header("Content-Type", "application/json");
        }
    
        // bad moon rising
        return response([ "statusCode" => 500, "message" => "some major server malfunction" ], 500)->header("Content-Type", "application/json");
    }

    function HealthCheck()
    {
        $compiler = new Compiler();
        if($compiler->healthCheck())
        {
            return response([
                "statusCode" => 200,
                "message" => "healthy"
            ], 200);
        }
        
        Log::error("Health check failed");
        Log::error($compiler->serialize());

        return response([
            "statusCode" => 400,
            "message" => "unhealthy"
        ], 400);
    }

    function compileCode($code, $libraries)
    {
        if($code == null || $libraries == null)
        {
            Log::debug("Compile: missing required code parameters");
    
            return [
                "statusCode" => 400,
                "message" => "missing required parameters",
            ];
        }
    
        if(strlen($code) > 50000)
        {
            Log::debug("Compile: code exceeds maximum limit");
            return [
                "statusCode" => 400,
                "message" => "code exceeds maximum limit",
            ];
        }
        
        if(!$this->validateLibraries($libraries))
        {
            return [
                "statusCode" => 400,
                "message" => "inavlid libraries set",
            ];
        }
        
        ksort($libraries);
        $hashedCode = hash("sha256", hashCode($code) . json_encode($libraries));

        if(env("COMPILER_CACHING", false))
        {
            try
            {
                $cachedCode = Redis::get($hashedCode);
            
                if(isset($cachedCode))
                {
                    Redis::expire($hashedCode, env("REDIS_TTL", 60));
                    Log::debug("Compile: cache hit", ["hashedCode" => $hashedCode]);
                    
                    $compiler = new Compiler();
                    $compiler->deserialize($cachedCode);
    
                    return [
                        "statusCode" => $compiler->getStatus(),
                        "hash" => $hashedCode,
                        "libraries" => $compiler->getLibraryVersions(),
                        "html" => $compiler->getHtml(),
                        "stdout" => $compiler->getOutput(),
                        "stderr" => $compiler->getErrorOutput(),
                    ];
                }
            }
            catch(Exception $e)
            {
                Log::emergency("Compiler Caching enabled and Redis failed.");
            }
            
            Log::debug("Compile: cache miss", ["hashedCode" => $hashedCode]);
        }
        
        if(Storage::directoryMissing("workspaces"))
        {
            Storage::makeDirectory("workspaces");
        }

        if(Storage::disk("local")->exists("workspaces"))
        {
            Storage::disk("local")->makeDirectory("workspaces");
        }
            
        $directoryName = "workspaces/" . Str::uuid();
        Storage::disk("local")->makeDirectory($directoryName);
        
        Log::debug("Compile: working directory created {$directoryName}");
        
        $compiler = new Compiler();
        
        $compiler->setCode($code);
        $compiler->setLibraryVersions($libraries);
        $compiler->setWorkingDirectory(Storage::disk("local")->path($directoryName));
        
        if($compiler->build())
        {
            if(env("COMPILER_CACHING", false))
            {
                try
                {
                    Redis::setex($hashedCode, env("REDIS_TTL", 60), $compiler->serialize());
                }
                catch(Exception $e)
                {
                    Log::emergency("Compiler Caching enabled but Redis failed");
                }
            }
                
            return [
                "statusCode" => 200,
                "hash" => $hashedCode,
                "html" => $compiler->getHtml(),
                "libraries" => $compiler->getLibraryVersions(),
                "stdout" => $compiler->getOutput(),
                "stderr" => $compiler->getErrorOutput(),
            ];
        }

        if(env("COMPILER_CACHING", false))
        {
            try
            {
                Redis::setex($hashedCode, env("REDIS_TTL", 60), $compiler->serialize());
            }
            catch(Exception $e)
            {
                Log::emergency("Compiler Caching enabled but Redis failed");
            }
        }
    
        return [
            "statusCode" => 400,
            "hash" => $hashedCode,
            "html" => $compiler->getHtml(),
            "libraries" => $compiler->getLibraryVersions(),
            "stdout" => $compiler->getOutput(),
            "stderr" => $compiler->getErrorOutput(),
        ];
    }
    
    function validateLibraries($libraries)
    {
        $libraryDirectory = env("PGETINKER_LIBS_DIRECTORY", "/opt/libs");
        
        if(!file_exists($libraryDirectory))
        {
            Log::error("Library directory does not exist: {$libraryDirectory}");
            return false;
        }
            
        if(count($libraries) == 0)
        {
            Log::error("No libraries are set.");
            return false;
        }

        foreach($libraries as $library => $version)
        {
            if(!file_exists("{$libraryDirectory}/{$library}/{$version}"))
            {
                Log::error("Library directory does not exist: {$libraryDirectory}/{$library}/{$version}");
                return false;
            }
        }

        return true;
    }
}


