# Online checkers
Jakub Binkowski 151921

### Description
The program enables players to play checkers online, all they need is to be conneted to the same network. It uses multithread TCP server written in C language. Every client is served in one new thread

### Server
Multithread server enables to maintain 5 games on the same time. If more players want to join the game, it displays the information that client should try again later. Server also is well prepared for situations when one player gets disconnected and then the second player gets the information that their opponent had just left the game. When the game finished normally both players get disconnected and they receive message about the winner. Server also validates whether coordinates of moves from a player are proper and sends them messages if there are some mistakes

Functions:
- `main` - setup and setting the connection with new clients
- `socketThread` - function to handle a communication with a client, one function working per one thread

### Client
TCP client gets connected and then works in an infinite loop where waits for messages from server. If the client gets any messages, then he displays the board or some information from the server. If server sends communicates that it needs player to make a move, then a client requests a player to input coordinates of a move

### Communication
When the connection is established, client is waiting in an infinite while loop for messages from server. Function which are used to communication:
- `send` - sending data
- `recv` - receiving data

Client can get these types of messages:
- "insert coordinates" - client has to request player to type coordinates of next move in terminal, server is waiting to receive these coordinates using `recv` function
- "sending board" - client is waiting to get an array with the current board, server sends the board
- "insert jump" - client has to request player to insert coordinates of next jump, server is waiting for these coordinates using `recv` function
- "read information" - client has to receive another text information (for example about the winner) from a server, server is sending it using `send` function
- "disconnect" - client is informed that their opponent had been disconnected, so it leaves the loop and the program finishes
- "exit" - client is informed that the game had just finished, so it leaves the loop and exits the program

### Game implementation
Server handles a game forcing white or black player to insert coordinates of the next move. In my checkers you always need to jump if it is possible. Kings work normally - they can move for a long distance.

Functions:
- `move_pawn` - moving pawns on a board
- `can_move_white`/ `can_move_black` - checks if given pawn can move to a given place
- `jump` - function which handles jumping
- `can_jump_white`/`can_jump_black` - checks if given pawn can jump to a given place
- `can_any_white_jump`/`can_any_black_jump` - checks if any pawn from a given color can do any jump
- `char_to_index` - changes letters to numbers to enable doing moves after inserting coordinates


### How to run:
All you need is gcc compiler on your computer and terminal, below simple commands to compile and run checkers:

Compilation:
```bash
gcc -pthread server.c -o server.out
gcc -pthread client.c -o client.out
```
Running the server:
```bash
./server.out
```
Running the client (ip_address is a server ip address, for example 10.0.0.1):
```bash
./client.out ip_address
```

