#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>

int board[8][8];

void show_board(int board[8][8]){
    for (int i=7;i>=0;i--){
        printf("  ---------------------------------\n");
        printf("%d |", i+1);
        for (int j=0;j<8;j++){
            if (board[i][j]==-1){
                printf("   |");
            }
            else if (board[i][j]==0){
                printf(" . |");
            }
            else if (board[i][j]==1){
                printf(" w |");
            }
            else if (board[i][j]==2){
                printf(" b |");
            }
            else if (board[i][j]==3){
                printf(" W |");
            }
            else if (board[i][j]==4){
                printf(" B |");
            }
        }
        printf("\n");
    }
    printf("  ---------------------------------\n");
    printf("    a   b   c   d   e   f   g   h\n");
}

int main(){

  char message[1024];
  char buffer[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  // Create the socket. 
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  //Configure settings of the server address
 // Address family is Internet 
  serverAddr.sin_family = AF_INET;

  //Set port number, using htons function 
  serverAddr.sin_port = htons(1100);

 //Set IP address
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    //strcpy(message,"Hello");
    int msg_scanf_size;


    for(;;){

        recv(clientSocket, buffer, 1024, 0);

        if(strcmp(buffer,"insert coordinates")==0){
            printf("Insert coordinates in format like c3-e4\n");
            scanf("%s", message);
            send(clientSocket, message, sizeof(message), 0);
        }
        else if(strcmp(buffer,"sending board")==0){
            printf("The board: \n");
            recv(clientSocket, board, sizeof(board), 0);
            show_board(board);
            memset(&board, 0, sizeof(board));
        }
        else if(strcmp(buffer,"read information")==0){
            printf("information from server: ");
            memset(&buffer, sizeof(buffer), 0);
            recv(clientSocket, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        }
        else if(strcmp(buffer,"insert jump")==0){
            printf("insert coordinates of a place where you want to jump next in format like e5\n");
            scanf("%s", message);
            send(clientSocket, message, sizeof(message), 0);
        }

        memset(&buffer, 0, sizeof(buffer));
        memset(&message, 0, sizeof (message));
    }

    close(clientSocket);
    
  return 0;
}
