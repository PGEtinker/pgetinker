<?php

use App\Models\Code;
use Illuminate\Http\Request;
use Illuminate\Mail\Markdown;
use Illuminate\Support\Facades\Route;

Route::get('/', function (Request $request)
{
    return view('home', [
        "code" => "",
        "share_thumb_url" => "",
    ]);
});

Route::get('/s/{slug}', function(Request $request, string $slug)
{
    $code = Code::where("slug", $slug)->firstOrFail();
    
    $code->view_count++;
    
    $code->save();

    return view("home", [
        "code" => $code->code,
        "share_thumb_url" => $code->thumb_url,
    ]);
});

Route::get("/disagree", function()
{
    return view("disagree");
});

Route::get("/player", function ()
{
    return view("player");
});

Route::get("/changelog", function()
{
    $path = base_path("/CHANGELOG.md");
    
    try
    {
        if(!file_exists($path))
            throw new Exception("path not found");
        
        $html = Markdown::parse(file_get_contents($path));

        return view("markdown", ["title" => "ChangeLog", "content" => $html]);
    }
    catch(Exception $e)
    {
        dd($e->getMessage());
    }
});
