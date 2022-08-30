# What does the program do?

It creates the client, server and dispatcher.

The client sends requests to the server and receives acknowledgement of the same.
The server reads the requests and then puts them in a queue. The server also sends back an acknowledgement of the same.
The dispatcher takes the requests from the queue and executes them under the threads, memory and file constraints.

# How to compile?

1. Unzip the source code.
2. Inside the directory, run `make` to build the project.

# How to run?

1. Compile the project.
2. Run the server and dispatcher by running `./build/main.out server [<PORT>] [-l <MAX_THREADS> <MAX_MEMORY_IN_KB> <MAX_FILES> <MAX_QUEUE_SIZE>]`
3. Send a request to the server by running `./build/main.out client [<PORT>]` and writing the request on stdin

Note: Default port in 9988

## Request format
The request format is as follows

First line contains the DLL name
Second line contains the function name
Third line contains an integer n (number of arguments)
Next n line contains n arguments, one in each line

For example, the following request
```json
{
  "dll_name": "/lib32/libm.so.6",
  "func_name": "sin",
  "num_args": 1,
  "func_args": ["1.57"]
}
```

is formatted as follows

```
/lib32/libm.so.6
sin
1
1.57
```

You can also store the request in a file (say input.txt)

```
/lib32/libm.so.6
sin
1
1.57

```

and then run `./build/main.out client [<PORT>] < input.txt`

# Example

1. Compile the project by running `make`

2. Start the server by running `./build/main.out server 1234` (1234 is the port)
  - To add limits on running the server run `./build/main.out server 1234 -l 8 1024 128 10` (8 threads, 1024KB memory, 128 files and max queue size is 10)

3. Create a file `input.txt` with following content 
```
/lib/x86_64-linux-gnu/libm.so.6
cos
1
1.57

```

4. In a new terminal at project folder, run `./build/main.out client 1234 < input.txt`
  - This will send the request stored in input.txt to the server and an acknowledgement will printed on the client side
  - The server side will print the status of reqeust and the result so obtained by running the request
