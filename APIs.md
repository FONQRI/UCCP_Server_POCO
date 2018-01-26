# APIs

## Signup

For sign up you will need to send a post request by json content (e.g):
```
{
	"username" : "username"
	"name : "behnam"
	"family" : "sabaghi" 
	"sex" : "male"
	"email" : "behnamsabaghi@gmail.com"
	"phoneNumber" : "09304912785"
	"berthday" :{
		"year" : 1995
		"month" : 10
		"day" : 4
	}
}
```
And if request be successful you will receive :
```
{
  "ok" : 1,
  "n" : 1
}
```

And if it fails you will receive error like this for duplicate keys :
```
{
  "ok" : 1,
  "n" : 0,
  "writeErrors" : [
    {
      "index" : 0,
      "code" : 11000,
      "errmsg" : "insertDocument :: caused by :: 11000 E11000 duplicate key error index: pocoTestDB.Users.$phoneNumber  dup key: { : \"09304912785\" }"
    }
  ]
}

```
