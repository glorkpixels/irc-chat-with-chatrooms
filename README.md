# irc chat with chatrooms

## Documentation

The server will be hosted on localhost
and its port is 3205. When the client connects successfully to the server, it will enter its
nickname initially. Then it is located in the common area such as lobby.
The commands are described in below can be used in the system:

* -list: Lists the currently available rooms with the name of the customers in it. If the
room is private, no details should be given.
* -create room_name: Creates a new specified room. Not more than one room with
the same name.
* -pcreate room_name: Creates a new specified private room. This type of room has
been protected with password. The system will ask to define a password.
* -enter room_name: Enter to the specified room. If the room is private, the client
must know the password for enter.
* -quit room_name: Quit from the room that you are in. You come back to the
common area.
* -msg message_body: Sends a message to room that you are in. Your and the
others' messages should look different in the room.
* -whoami: Shows your own nickname information.
* -exit: Exit the program.



## Running
my server and clients are compiling however I couldn't manage to receive starting parameters from user I think which is caused by my client_handler() function. If I had manage to do that it would run smoothly.
it .runs with lpthread

'''sh
gcc 2400000145_server.c -o server.out -lpthread

gcc 2400000145_client.c -o client.out -lpthread
'''



## Update
 I think reason of segmentation fault on my function is I didn't memset bunch of things I create. Which I didn't exactly know I had to do. With memsets probablity of working is high may be with some couple more bugs. I didn't updated my code according to that but I think I sorted it out as  whole on my extra work. **update(30/12/2019 20:30)**
