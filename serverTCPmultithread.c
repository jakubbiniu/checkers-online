#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include <signal.h>

char client_message[1024];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int max_num_games = 5;

int board[8][8] = {{1,-1,1,-1,1,-1,1,-1},{-1,1,-1,1,-1,1,-1,1},{1,-1,1,-1,1,-1,1,-1},{-1,0,-1,0,-1,0,-1,0},{0,-1,0,-1,0,-1,0,-1},{-1,2,-1,2,-1,2,-1,2},{2,-1,2,-1,2,-1,2,-1},{-1,2,-1,2,-1,2,-1,2}};

int boards[5][8][8] = {
    {
        {0,-1,0,-1,0,-1,0,-1},
        {-1,0,-1,0,-1,1,-1,0},
        {0,-1,0,-1,1,-1,0,-1},
        {-1,0,-1,0,-1,0,-1,0},
        {0,-1,2,-1,0,-1,0,-1},
        {-1,0,-1,0,-1,2,-1,0},
        {0,-1,0,-1,0,-1,0,-1},
        {-1,0,-1,0,-1,0,-1,0}
    },
    {
        {1,-1,1,-1,1,-1,1,-1},
        {-1,1,-1,1,-1,1,-1,1},
        {1,-1,1,-1,1,-1,1,-1},
        {-1,0,-1,0,-1,0,-1,0},
        {0,-1,0,-1,0,-1,0,-1},
        {-1,2,-1,2,-1,2,-1,2},
        {2,-1,2,-1,2,-1,2,-1},
        {-1,2,-1,2,-1,2,-1,2}
    },
    {
        {1,-1,1,-1,1,-1,1,-1},
        {-1,1,-1,1,-1,1,-1,1},
        {1,-1,1,-1,1,-1,1,-1},
        {-1,0,-1,0,-1,0,-1,0},
        {0,-1,0,-1,0,-1,0,-1},
        {-1,2,-1,2,-1,2,-1,2},
        {2,-1,2,-1,2,-1,2,-1},
        {-1,2,-1,2,-1,2,-1,2}
    },
    {
        {1,-1,1,-1,1,-1,1,-1},
        {-1,1,-1,1,-1,1,-1,1},
        {1,-1,1,-1,1,-1,1,-1},
        {-1,0,-1,0,-1,0,-1,0},
        {0,-1,0,-1,0,-1,0,-1},
        {-1,2,-1,2,-1,2,-1,2},
        {2,-1,2,-1,2,-1,2,-1},
        {-1,2,-1,2,-1,2,-1,2}
    },
    {
        {1,-1,1,-1,1,-1,1,-1},
        {-1,1,-1,1,-1,1,-1,1},
        {1,-1,1,-1,1,-1,1,-1},
        {-1,0,-1,0,-1,0,-1,0},
        {0,-1,0,-1,0,-1,0,-1},
        {-1,2,-1,2,-1,2,-1,2},
        {2,-1,2,-1,2,-1,2,-1},
        {-1,2,-1,2,-1,2,-1,2}
    }
};

int all_white_pawns[5]={2,12,12,12,12};
int all_black_pawns[5]={2,12,12,12,12};
int all_white_kings[5]={0,0,0,0,0};
int all_black_kings[5]={0,0,0,0,0};

int games[5] = {0, 0, 0, 0, 0}; //number of players connected to a game
int to_move[5] = {0, 0, 0, 0, 0}; // 0 - white, 1 - black
int game_status[5] = {0, 0, 0, 0, 0}; // 1 - white won, 2- black won, 0 - currently no winner, 3 - disconnect

// funkcja przesuwająca pionki
void move_pawn(int board[8][8], int x1, int y1, int x2, int y2){
    x1--;x2--;y1--;y2--;
    int p=board[x1][y1];
    board[x1][y1]=0;
    board[x2][y2]=p;
}

// sprawdzanie czy dany bialy pion moze wykonac dany ruch
int can_move_white(int board[8][8], int x1, int y1, int x2, int y2){
    x1=x1-1;
    x2=x2-1;
    y1=y1-1;
    y2=y2-1;
    if (x1<0 || x1>7 || x2<0 || x2>7 || y1<0 || y1>7 || y2<0 || y2>7){
        return 0;
    }
    if (board[x2][y2]!=0){
        return 0;
    }
    if (abs(x2-x1)!=abs(y2-y1)){
            return 0;
        }
    if (board[x1][y1]==1 && abs(y2-y1)==1 && x2-x1==1){
        return 1;
    }
    if (board[x1][y1]==3){
        if (x2>x1 && y2>y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1+abs(i-x1);
                if (board[i][j]!=0){
                    return 0;
                }
            }
            return 1;
        }
        if (x2>x1 && y2<y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1-abs(i-x1);
                if (board[i][j]!=0){
                    return 0;
                }
            }
            return 1;
        }
        if (x2<x1 && y2>y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1+abs(x1-i);
                if (board[i][j]!=0){
                    return 0;
                }
            }
            return 1;
        }
        if (x2<x1 && y2<y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1-abs(x1-i);
                if (board[i][j]!=0){
                    return 0;
                }
            }
            return 1;
        }
    }
    return 0;
}

// sprawdzanie czy czarny pion moze wykonać ruch
int can_move_black(int board[8][8], int x1, int y1, int x2, int y2){
    x1=x1-1;
    x2=x2-1;
    y1=y1-1;
    y2=y2-1;
    if (x1<0 || x1>7 || x2<0 || x2>7 || y1<0 || y1>7 || y2<0 || y2>7){
        return 0;
    }
    if (board[x2][y2]!=0){
        return 0;
    }
    if (abs(x2-x1)!=abs(y2-y1)){
            return 0;
        }
    if (board[x1][y1]==2 && abs(y1-y2)==1 && x1-x2==1){
        return 1;
    }
    if (board[x1][y1]==4){
        if (x2>x1 && y2>y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1+abs(i-x1);
                if (board[i][j]!=0){
                    return 0;
                }
            }
            return 1;
        }
        if (x2>x1 && y2<y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1-abs(i-x1);
                if (board[i][j]!=0){
                    return 0;
                }
            }
            return 1;
        }
        if (x2<x1 && y2>y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1+abs(x1-i);
                if (board[i][j]!=0){
                    return 0;
                }
            }
            return 1;
        }
        if (x2<x1 && y2<y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1-abs(x1-i);
                if (board[i][j]!=0){
                    return 0;
                }
            }
            return 1;
        }
    }
    return 0;
}
// bicie
void jump(int board[8][8], int x1, int y1, int x2, int y2, int *pawns, int *kings){
    x1--;
    x2--;
    y1--;
    y2--;
    int p = board[x1][y1];
    if (board[x1][y1]==1 || board[x1][y1]==2){
        int x3=(x1+x2)/2;
        int y3=(y1+y2)/2;
        if (board[x3][y3]==1 || board[x3][y3]==2){
            (*pawns)--;
        }
        else{
            (*kings)--;
        }
        board[x3][y3]=0;
        board[x2][y2]=p;
        board[x1][y1]=0;
    }
    else if (board[x1][y1]==3 || board[x1][y1]==4){
        if (x2>x1 && y2>y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1+abs(i-x1);
                if (board[i][j]!=0){
                    if (board[i][j]==1 || board[i][j]==2){
                        (*pawns)--;
                    }
                    else{
                        (*kings)--;
                    }
                    board[i][j]=0;
                    board[x2][y2]=p;
                    board[x1][y1]=0;
                    return;
                }
            }
        }
        if (x2>x1 && y2<y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1-abs(i-x1);
                if (board[i][j]!=0){
                    if (board[i][j]==1 || board[i][j]==2){
                        (*pawns)--;
                    }
                    else{
                        (*kings)--;
                    }
                    board[i][j]=0;
                    board[x2][y2]=p;
                    board[x1][y1]=0;
                    return;
                }
            }
        }
        if (x2<x1 && y2>y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1+abs(x1-i);
                if (board[i][j]!=0){
                    if (board[i][j]==1 || board[i][j]==2){
                        (*pawns)--;
                    }
                    else{
                        (*kings)--;
                    }
                    board[i][j]=0;
                    board[x2][y2]=p;
                    board[x1][y1]=0;
                    return;
                }
            }
        }
        if (x2<x1 && y2<y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1-abs(x1-i);
                if (board[i][j]!=0){
                    if (board[i][j]==1 || board[i][j]==2){
                        (*pawns)--;
                    }
                    else{
                        (*kings)--;
                    }
                    board[i][j]=0;
                    board[x2][y2]=p;
                    board[x1][y1]=0;
                    return;
                }
            }
        }
    }
    return;
}

// sprawdzanie czy dany bialy pion moze bic
int can_jump_white(int board[8][8], int x1, int y1, int x2, int y2){
    x1--;
    x2--;
    y1--;
    y2--;
    int x3 = (x1+x2)/2;
    int y3 = (y1+y2)/2;
    if (x1<0 || x1>7 || x2<0 || x2>7 || y1<0 || y1>7 || y2<0 || y2>7){
        return 0;
    }
    if (board[x2][y2]!=0){
        return 0;
    }
    if (abs(x2-x1)!=abs(y2-y1)){
            return 0;
        }
    if (board[x1][y1]==1 && (board[x3][y3]==2 || board[x3][y3]==4) && abs(x1-x2)==2 && abs(y2-y1)==2){
        return 1;
    }
    if (board[x1][y1]==3){
        int c=0;
        if (x2>x1 && y2>y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1+abs(i-x1);
                if (board[i][j]==2 || board[i][j]==4){
                    c++;
                }
                else if (board[i][j]==1 || board[i][j]==3){
                    return 0;
                }
            }
            if (c==1){
                return 1;
            }
            return 0;
        }
        else if (x2>x1 && y2<y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1-abs(i-x1);
                if (board[i][j]==2 || board[i][j]==4){
                    c++;
                }
                 else if (board[i][j]==1 || board[i][j]==3){
                    return 0;
                }
            }
            if (c==1){
                return 1;
            }
            return 0;
        }
        else if (x2<x1 && y2>y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1+abs(x1-i);
                if (board[i][j]==2 || board[i][j]==4){
                    c++;
                }
                 else if (board[i][j]==1 || board[i][j]==3){
                    return 0;
                }
            }
            if (c==1){
                return 1;
            }
            return 0;
        }
        else if (x2<x1 && y2<y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1-abs(x1-i);
                if (board[i][j]==2 || board[i][j]==4){
                    c++;
                }
                 else if (board[i][j]==1 || board[i][j]==3){
                    return 0;
                }
            }
            if (c==1){
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

// sprawdzanie czy czarny pion moze bic
int can_jump_black(int board[8][8], int x1, int y1, int x2, int y2){
    x1--;
    x2--;
    y1--;
    y2--;
    int x3 = (x1+x2)/2;
    int y3 = (y1+y2)/2;
    if (x1<0 || x1>7 || x2<0 || x2>7 || y1<0 || y1>7 || y2<0 || y2>7){
        return 0;
    }
    if (board[x2][y2]!=0){
        return 0;
    }
    if (abs(x2-x1)!=abs(y2-y1)){
            return 0;
        }
    if (board[x1][y1]==2 && (board[x3][y3]==1 || board[x3][y3]==3) && abs(x1-x2)==2 && abs(y2-y1)==2){
        return 1;
    }
    if (board[x1][y1]==4){
        int c=0;
        if (x2>x1 && y2>y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1+abs(i-x1);
                if (board[i][j]==1 || board[i][j]==3){
                    c++;
                }
                else if (board[i][j]==2 || board[i][j]==4){
                    return 0;
                }
            }
            if (c==1){
                return 1;
            }
            return 0;
        }
        if (x2>x1 && y2<y1){
            for (int i=x1+1;i<x2;i++){
                int j=y1-abs(i-x1);
                if (board[i][j]==1 || board[i][j]==3){
                    c++;
                }
                 else if (board[i][j]==2 || board[i][j]==4){
                    return 0;
                }
            }
            if (c==1){
                return 1;
            }
            return 0;
        }
        if (x2<x1 && y2>y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1+abs(x1-i);
                if (board[i][j]==1 || board[i][j]==3){
                    c++;
                }
                 else if (board[i][j]==2 || board[i][j]==4){
                    return 0;
                }
            }
            if (c==1){
                return 1;
            }
            return 0;
        }
        if (x2<x1 && y2<y1){
            for (int i=x1-1;i>x2;i--){
                int j=y1-abs(x1-i);
                if (board[i][j]==1 || board[i][j]==3){
                    c++;
                }
                 else if (board[i][j]==2 || board[i][j]==4){
                    return 0;
                }
            }
            if (c==1){
                return 1;
            }
            return 0;
        }
    }
    return 0;
}
// sprawdzanie czy jakikolwiek bialy pion moze bic
int can_any_white_jump(int board[8][8]){
    for (int i=1;i<9;i++){
        for (int j=1;j<9;j++){
            if (board[i-1][j-1]==1){
                if (can_jump_white(board,i,j,i+2,j+2)==1 || can_jump_white(board,i,j,i+2,j-2)==1 || can_jump_white(board,i,j,i-2,j+2)==1 || can_jump_white(board,i,j,i-2,j-2)==1){
                    return 1;
                }
            }
            else if (board[i-1][j-1]==3){
                for (int a=1;a<9;a++){
                    for (int b=1;b<9;b++){
                        if (can_jump_white(board,i,j,a,b)==1){
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
// sprawdzanie czy jakikolwiek czarny pion moze bic
int can_any_black_jump(int board[8][8]){
    for (int i=1;i<9;i++){
        for (int j=1;j<9;j++){
            if (board[i-1][j-1]==2){
                if (can_jump_black(board,i,j,i+2,j+2)==1 || can_jump_black(board,i,j,i+2,j-2)==1 || can_jump_black(board,i,j,i-2,j+2)==1 || can_jump_black(board,i,j,i-2,j-2)==1){
                    return 1;
                }
            }
            else if (board[i-1][j-1]==4){
                for (int a=1;a<9;a++){
                    for (int b=1;b<9;b++){
                        if (can_jump_black(board,i,j,a,b)==1){
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int char_to_index(char c) {
    return c - 'a' + 1;
}

void * socketThread(void *arg){
    int newSocket = *((int *)arg);
    printf("new thread %d\n",newSocket);
    int n;
    int game_found = 0;
    char color[128];
    int game_num;
    int x_1, x_2, y_1, y_2;
    char x_1_char, x_2_char;

    for (int i = 0; i < 5;i++){
        if(games[i]<2){
        game_num = i;
        game_found = 1;
        if(games[i]==1){
            strcpy(color,"black");
        }
        else if(games[i]==0){
            strcpy(color,"white");
        }
        games[i]++;
        break;
        }
    }

    if(game_found==0){
        strcpy(buffer, "read information");
        send(newSocket, buffer, sizeof(buffer), 0);
        memset(&buffer, 0, sizeof(buffer));
        strcpy(buffer, "servers are busy");
        send(newSocket, buffer, sizeof(buffer), 0);
        memset(&buffer, 0, sizeof(buffer));
        close(newSocket);
        pthread_exit(NULL);
    }
    else{
        strcpy(buffer, "read information");
        send(newSocket, buffer, sizeof(buffer), 0);
        memset(&buffer, 0, sizeof(buffer));
        strcpy(buffer, "connected to a game");
        send(newSocket, buffer, sizeof(buffer), 0);
        memset(&buffer, 0, sizeof(buffer));
    }

    strcpy(buffer, "read information");
    send(newSocket, buffer, sizeof(buffer), 0);
    memset(&buffer, 0, sizeof(buffer));
    strcpy(buffer, color);
    send(newSocket, buffer, sizeof(buffer), 0);
    memset(&buffer, 0, sizeof(buffer));
    char game_num_string[128];
    sprintf(game_num_string, "%d", game_num);
    strcpy(buffer, "read information");
    send(newSocket, buffer, sizeof(buffer), 0);
    memset(&buffer, 0, sizeof(buffer));
    strcpy(buffer, game_num_string);
    send(newSocket, buffer, sizeof(buffer), 0);
    memset(&buffer, 0, sizeof(buffer));

    for (;;){
        if(game_status[game_num]==3){
            send(newSocket, "disconnect", 1024, 0);
            game_status[game_num] = 0;
            games[game_num] = 0;
            all_black_kings[game_num] = 0;
            all_black_pawns[game_num] = 12;
            all_white_kings[game_num] = 0;
            all_white_pawns[game_num] = 12;
            for (int i = 0; i<8;i++){
                for (int j = 0; j < 8;j++){
                    boards[game_num][i][j] = board[i][j];
                }
            }
            to_move[game_num] = 0;
        }
        else if (to_move[game_num] == 0 && strcmp(color,"white")==0 && (all_white_pawns[game_num]+all_white_kings[game_num])>0 && (all_black_pawns[game_num]+all_black_kings[game_num])>0){
            strcpy(buffer, "sending board");
            send(newSocket, buffer, sizeof(buffer), 0);
            memset(&buffer, 0, sizeof(buffer));
            send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
            if(can_any_white_jump(boards[game_num])==1){
                send(newSocket, "insert coordinates", 1024, 0);
                n = recv(newSocket, client_message, sizeof(client_message),0);
                if(n<=0){
                    game_status[game_num] = 3;
                    printf("Client disconnected: %d\n", newSocket);
                    close(newSocket);
                    pthread_exit(NULL);
                    break;
                }
                sscanf(client_message, "%c%d-%c%d", &x_1_char, &x_1, &x_2_char, &x_2);
                y_1 = char_to_index(x_1_char);
                y_2 = char_to_index(x_2_char);
                while (can_jump_white(boards[game_num],x_1,y_1,x_2,y_2)==0){
                    send(newSocket, "read information", 1024, 0);
                    // send(newSocket, "wrong coordinates", 1024, 0);
                    send(newSocket, "insert coordinates", 1024, 0);
                    n = recv(newSocket, client_message, sizeof(client_message),0);
                    if(n<=0){
                        game_status[game_num] = 3;
                        printf("Client disconnected: %d\n", newSocket);
                        close(newSocket);
                        pthread_exit(NULL);
                        break;
                    }   
                    sscanf(client_message, "%c%d-%c%d", &x_1_char, &x_1, &x_2_char, &x_2);
                    y_1 = char_to_index(x_1_char);
                    y_2 = char_to_index(x_2_char);
                }
                jump(boards[game_num],x_1,y_1,x_2,y_2,&all_black_pawns[game_num],&all_black_kings[game_num]);
                for (int i=0;i<8;i++){
                    if (boards[game_num][7][i]==1){
                        boards[game_num][7][i]=3;
                        all_white_pawns[game_num]--;
                        all_white_kings[game_num]++;
                    }
                }
                send(newSocket, "sending board", 1024, 0);
                send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
                x_1 = x_2;
                y_1 = y_2;
                while (can_jump_white(boards[game_num],x_1,y_1,x_1+2,y_1+2)==1 || can_jump_white(boards[game_num],x_1,y_1,x_1+2,y_1-2)==1 || can_jump_white(boards[game_num],x_1,y_1,x_1-2,y_1-2)==1 || can_jump_white(boards[game_num],x_1,y_1,x_1-2,y_1+2)==1){
                    send(newSocket, "insert jump", 1024, 0);
                    n = recv(newSocket, client_message, sizeof(client_message),0);
                    if(n<=0){
                        game_status[game_num] = 3;
                        printf("Client disconnected: %d\n", newSocket);
                        close(newSocket);
                        pthread_exit(NULL);
                        break;
                    }
                    sscanf(client_message, "%c%d",&x_2_char,&x_2);
                    y_2 = char_to_index(x_2_char);
                    if (can_jump_white(boards[game_num], x_1, y_1, x_2, y_2) == 1){
                        jump(boards[game_num], x_1, y_1, x_2, y_2, &all_black_pawns[game_num], &all_black_kings[game_num]);
                        for (int i = 0; i < 8; i++){
                            if (boards[game_num][7][i] == 1){
                                boards[game_num][7][i] = 3;
                                all_white_pawns[game_num]--;
                                all_white_kings[game_num]++;
                            }
                        }
                        send(newSocket, "sending board", 1024, 0);
                        send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
                        x_1 = x_2;
                        y_1 = y_2;
                    }
                }
                to_move[game_num] = 1;
            }
            else{
                while (1){
                    send(newSocket, "insert coordinates", 1024, 0);
                    n = recv(newSocket, client_message, sizeof(client_message),0);
                    if(n<=0){
                        game_status[game_num] = 3;
                        printf("Client disconnected: %d\n", newSocket);
                        close(newSocket);
                        pthread_exit(NULL);
                        break;
                    }
                    sscanf(client_message, "%c%d-%c%d", &x_1_char, &x_1, &x_2_char, &x_2);
                    y_1 = char_to_index(x_1_char);
                    y_2 = char_to_index(x_2_char);
                    if (can_move_white(boards[game_num],x_1,y_1,x_2,y_2)==1){
                        move_pawn(boards[game_num],x_1,y_1,x_2,y_2);
                        for (int i=0;i<8;i++){
                            if (boards[game_num][7][i]==1){
                                boards[game_num][7][i]=3;
                                all_white_kings[game_num]++;
                                all_white_pawns[game_num]--;
                            }
                        }
                        send(newSocket, "sending board", 1024, 0);
                        send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
                        to_move[game_num] = 1;
                        break;
                    }
                }
            }
        }

        else if(to_move[game_num] == 1 && strcmp(color,"black")==0 && (all_white_pawns[game_num]+all_white_kings[game_num])>0 && (all_black_pawns[game_num]+all_black_kings[game_num])>0){
            printf("%d %d\n", all_black_kings[game_num], all_black_pawns[game_num]);
            send(newSocket, "sending board", 1024, 0);
            send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
            if(can_any_black_jump(boards[game_num])==1){
                send(newSocket, "insert coordinates", 1024, 0);
                n = recv(newSocket, client_message, sizeof(client_message),0);
                if(n<=0){
                    game_status[game_num] = 3;
                    printf("Client disconnected: %d\n", newSocket);
                    close(newSocket);
                    pthread_exit(NULL);
                    break;
                }
                sscanf(client_message, "%c%d-%c%d", &x_1_char, &x_1, &x_2_char, &x_2);
                y_1 = char_to_index(x_1_char);
                y_2 = char_to_index(x_2_char);
                while (can_jump_black(boards[game_num],x_1,y_1,x_2,y_2)==0){
                    send(newSocket, "read information", 1024, 0);
                    send(newSocket, "wrong coordinates", 1024, 0);
                    send(newSocket, "insert coordinates", 1024, 0);
                    n = recv(newSocket, client_message, sizeof(client_message),0);
                    if(n<=0){
                        game_status[game_num] = 3;
                        printf("Client disconnected: %d\n", newSocket);
                        close(newSocket);
                        pthread_exit(NULL);
                        break;
                    }
                    sscanf(client_message, "%c%d-%c%d", &x_1_char, &x_1, &x_2_char, &x_2);
                    y_1 = char_to_index(x_1_char);
                    y_2 = char_to_index(x_2_char);
                }
                jump(boards[game_num],x_1,y_1,x_2,y_2,&all_white_pawns[game_num],&all_white_kings[game_num]);
                for (int i=0;i<8;i++){
                    if (boards[game_num][0][i]==2){
                        boards[game_num][0][i]=4;
                        all_black_pawns[game_num]--;
                        all_black_kings[game_num]++;
                    }
                }
                send(newSocket, "sending board", 1024, 0);
                send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
                x_1 = x_2;
                y_1 = y_2;
                while (can_jump_black(boards[game_num],x_1,y_1,x_1+2,y_1+2)==1 || can_jump_black(boards[game_num],x_1,y_1,x_1+2,y_1-2)==1 || can_jump_black(boards[game_num],x_1,y_1,x_1-2,y_1-2)==1 || can_jump_black(boards[game_num],x_1,y_1,x_1-2,y_1+2)==1){
                    send(newSocket, "insert jump", 1024, 0);
                    n = recv(newSocket, client_message, sizeof(client_message),0);
                    if(n<=0){
                        game_status[game_num] = 3;
                        printf("Client disconnected: %d\n", newSocket);
                        close(newSocket);
                        pthread_exit(NULL);
                        break;
                    }
                    sscanf(client_message, "%c%d",&x_2_char,&x_2);
                    y_2 = char_to_index(x_2_char);
                    if (can_jump_black(boards[game_num], x_1, y_1, x_2, y_2) == 1){
                        jump(boards[game_num], x_1, y_1, x_2, y_2, &all_white_pawns[game_num], &all_white_kings[game_num]);
                        for (int i = 0; i < 8; i++){
                            if (boards[game_num][0][i] == 2){
                                boards[game_num][0][i] = 4;
                                all_black_pawns[game_num]--;
                                all_black_kings[game_num]++;
                            }
                        }
                        send(newSocket, "sending board", 1024, 0);
                        send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
                        x_1 = x_2;
                        y_1 = y_2;
                    }
                }
                to_move[game_num] = 0;
            }
            else{
                while (1){
                    send(newSocket, "insert coordinates", 1024, 0);
                    n = recv(newSocket, client_message, sizeof(client_message),0);
                    if(n<=0){
                        game_status[game_num] = 3;
                        printf("Client disconnected: %d\n", newSocket);
                        close(newSocket);
                        pthread_exit(NULL);
                        break;
                    }
                    sscanf(client_message, "%c%d-%c%d", &x_1_char, &x_1, &x_2_char, &x_2);
                    y_1 = char_to_index(x_1_char);
                    y_2 = char_to_index(x_2_char);
                    if (can_move_black(boards[game_num],x_1,y_1,x_2,y_2)==1){
                        move_pawn(boards[game_num],x_1,y_1,x_2,y_2);
                        for (int i=0;i<8;i++){
                            if (boards[game_num][0][i]==2){
                                boards[game_num][0][i]=4;
                                all_black_kings[game_num]++;
                                all_black_pawns[game_num]--;
                            }
                        }
                        send(newSocket, "sending board", 1024, 0);
                        send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
                        to_move[game_num] = 0;
                        break;
                    }
                }
            }
        }
        else if(to_move[game_num]==1 && strcmp(color,"black")==0 && (all_black_kings[game_num]+all_black_pawns[game_num])==0){
            send(newSocket, "sending board", 1024, 0);
            send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
            send(newSocket, "read information", 1024, 0);
            send(newSocket, "white player won", 1024, 0);
            game_status[game_num] = 1;
            break;
        }
        else if(to_move[game_num]==1 && strcmp(color,"white")==0 && game_status[game_num]==1 && (all_black_kings[game_num]+all_black_pawns[game_num])==0){
            send(newSocket, "read information", 1024, 0);
            send(newSocket, "white player won", 1024, 0);
            all_black_kings[game_num] = 0;
            all_black_pawns[game_num] = 12;
            all_white_kings[game_num] = 0;
            all_white_pawns[game_num] = 12;
            for (int i = 0; i<8;i++){
                for (int j = 0; j < 8;j++){
                    boards[game_num][i][j] = board[i][j];
                }
            }
            to_move[game_num] = 0;
            games[game_num] = 0;
            game_status[game_num] = 0;
            break;
        }
        else if(to_move[game_num]==0 && strcmp(color,"white")==0 && (all_white_kings[game_num]+all_white_pawns[game_num])==0){
            send(newSocket, "sending board", 1024, 0);
            send(newSocket, boards[game_num], sizeof(boards[game_num]), 0);
            send(newSocket, "read information", 1024, 0);
            send(newSocket, "black player won", 1024, 0);
            game_status[game_num] = 2;
            break;
        }
        else if(to_move[game_num]==0 && strcmp(color,"black")==0 && game_status[game_num]==2 && (all_white_kings[game_num]+all_white_pawns[game_num])==0){
            send(newSocket, "read information", 1024, 0);
            send(newSocket, "black player won", 1024, 0);
            all_black_kings[game_num] = 0;
            all_black_pawns[game_num] = 12;
            all_white_kings[game_num] = 0;
            all_white_pawns[game_num] = 12;
            for (int i = 0; i<8;i++){
                for (int j = 0; j < 8;j++){
                    boards[game_num][i][j] = board[i][j];
                }
            }
            to_move[game_num] = 0;
            games[game_num] = 0;
            game_status[game_num] = 0;
            break;
        }
    }
        printf("Exit socketThread %d\n",newSocket);
        send(newSocket, "exit", 1024, 0);
        close(newSocket);   
        pthread_exit(NULL);
}


int main(){
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  //Create the socket. 
  serverSocket = socket(PF_INET, SOCK_STREAM, 0);

  // Configure settings of the server address struct
  // Address family = Internet 
  serverAddr.sin_family = AF_INET;

  //Set port number, using htons function to use proper byte order 
  serverAddr.sin_port = htons(1100);

  //Set IP address to localhost 
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);


  //Set all bits of the padding field to 0 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  //Bind the address struct to the socket 
  bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  //Listen on the socket
  if(listen(serverSocket,50)==0)
    printf("Listening\n");
  else
    printf("Error\n");
    pthread_t thread_id;

    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        if( pthread_create(&thread_id, NULL, socketThread, &newSocket) != 0 )
           printf("Failed to create thread\n");

        pthread_detach(thread_id);
        //pthread_join(thread_id,NULL);
    }
  return 0;
}
