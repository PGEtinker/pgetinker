<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{ $title }} | PGEtinker</title>
    <link rel="icon" type="text/svg" href="/favicon.svg">
    
    <link rel="stylesheet" type="text/css" href="{{ Vite::asset('resources/css/normalize.scss')}}">
    <link rel="stylesheet" type="text/css" href="{{ Vite::asset('resources/css/markdown.scss')}}">
</head>
<body>
    <div class="container">
        {!! $content !!}
    </div>
    
    @include("shared.analytics")
</body>
</html>