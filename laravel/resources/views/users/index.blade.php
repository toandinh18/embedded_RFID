@extends('base')

@section('main')

<div class="row">
<div class="col-sm-12">
    <h1 class="display-3">Member Information</h1>    
  <table class="table table-striped">
    <thead>
        <tr>
          <td>ID</td>
          <td>UID</td>
          <td>Name</td>
          <td>Phone</td>
          <td>Authorized</td>
          <td>Time</td>
          <td>Edit</td>
          <td>Delete</td>
        </tr>
    </thead>
    <tbody>
        @foreach($users as $user)
        <tr>
            <td>{{$user->id}}</td>
            <td>{{$user->UID}}</td>
            <td>{{$user->Name}}</td>
            <td>{{$user->Phone_number}}</td>
            <td>{{$user->Authorized}}</td>
            <td>{{$user->Time}}</td>
            <td>
                <a href="{{ route('users.edit',$user->id)}}" class="btn btn-primary">Edit</a>
            </td>
            <td>
                <form action="{{ route('users.destroy', $user->id)}}" method="post">
                  @csrf
                  @method('DELETE')
                  <button class="btn btn-danger" type="submit">Delete</button>
                </form>
            </td>
        </tr>
        @endforeach
    </tbody>
  </table>
</div>
</div>
@endsection