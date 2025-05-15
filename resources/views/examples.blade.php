<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{ $title }} | PGEtinker</title>
    <link rel="icon" type="text/svg" href="/favicon.svg">
    
    <link rel="stylesheet" type="text/css" href="{{ Vite::asset('resources/css/normalize.scss')}}">

    <link rel="stylesheet" type="text/css" href="{{ Vite::asset('resources/css/markdown-dark.scss')}}" id="dark-theme">
    <link rel="stylesheet" type="text/css" href="{{ Vite::asset('resources/css/markdown-light.scss')}}" id="light-theme" disabled>
    <link rel="stylesheet" type="text/css" href="{{ Vite::asset('resources/css/markdown.scss')}}">
</head>
<body class="markdown-body {!! $framed !!}">
    <div class="container">
        {!! $content !!}
    </div>
    
    @include("shared.analytics")
    @vite('resources/js/markdown.js')
</body>
</html>