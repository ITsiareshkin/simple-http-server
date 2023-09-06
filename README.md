# IPK Projekt 1

 The task is to create a server in C/C ++ language communicating via HTTP, which will provide various information about the system such as:

 1. Obtaining a domain name
 2. Obtaining CPU name
 3. Current CPU load

### Author

* Ivan Tsiareshkin (xtsiar00)

### Compilation

* The server is compiled using the next command, which creates the hinfosvc executable.

```
make all
```

* To create .zip archive:
```
make pack
```

* To delete executable and archive:
```
make pack
```

## Usage

1. Obtaining a domain name
```
./hinfosvc 12345 & curl http://localhost:12345/hostname

or

./hinfosvc 12345 & GET http://localhost:12345/hostname
```

 2. Obtaining CPU name
```
./hinfosvc 12345 & curl http://localhost:12345/cpu-name

or

./hinfosvc 12345 & GET http://localhost:12345/cpu-name
```

 3. Current CPU load
 ```
 ./hinfosvc 12345 & curl http://localhost:12345/load

 or

 ./hinfosvc 12345 & GET http://localhost:12345/load
 ```

 An argument "12345" is indicating the local port on which server will listen for requests.
