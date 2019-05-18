<?php

namespace App;

use Illuminate\Database\Eloquent\Model;

class User extends Model
{
    protected $fillable = ['Name', 'UID','status','Authorized','Phone_number'];
}
