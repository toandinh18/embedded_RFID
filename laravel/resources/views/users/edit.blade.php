@extends('base')
@section('main')

<div class="row">
    <div class="col-sm-8 offset-sm-2">
        <h1 class="display-0.5">Edit the member infor</h1>
        <form method="post" action="{{ route('users.update', $users->id) }}">
            @method('PATCH') 
            @csrf
            <div class="form-group">

                <label for="Name">Name:</label>
                <input type="text" class="form-control" name="Name" value={{ $users->Name }} />
            </div>

            <div class="form-group">
                <label for="Phone_number"> Phone :</label>
                <input type="text" class="form-control" name="Phone_number" value={{ $users->Phone_number }} />
            </div>
            <div class="form-group">
                <label for="Authorized"> Authorized :</label>
                <input type="text" class="form-control" name="Authorized" value={{ $users->Authorized }} />
            </div>
            <button type="submit" class="btn btn-primary">Update</button>
        </form>
    </div>
</div>
@endsection