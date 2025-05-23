<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class Code extends Model
{
    use HasFactory;
    
    protected $casts = [
        'library_versions' => 'array', // Automatically converts JSON to PHP array and back
    ];    
}
