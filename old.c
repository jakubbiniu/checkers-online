#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int board[8][8] = {{1,-1,1,-1,1,-1,1,-1},{-1,1,-1,1,-1,1,-1,1},{1,-1,1,-1,1,-1,1,-1},{-1,0,-1,0,-1,0,-1,0},{0,-1,0,-1,0,-1,0,-1},{-1,2,-1,2,-1,2,-1,2},{2,-1,2,-1,2,-1,2,-1},{-1,2,-1,2,-1,2,-1,2}};
// int board[8][8] = {{4,-1,4,-1,0,-1,4,-1},{-1,0,-1,0,-1,0,-1,0},{2,-1,0,-1,0,-1,0,-1},{-1,0,-1,0,-1,0,-1,0},{0,-1,0,-1,0,-1,0,-1},{-1,0,-1,0,-1,0,-1,0},{0,-1,0,-1,0,-1,0,-1},{-1,3,-1,3,-1,3,-1,3}};
int x_1, x_2, y_1, y_2;
int white_pawns=12;
int black_pawns=12;
int white_kings=0;
int black_kings=0;
int game_over = 0;
char x_1_char, x_2_char;

// funkcja pokazująca plansze
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
                printf(" b |");
            }
            else if (board[i][j]==2){
                printf(" c |");
            }
            else if (board[i][j]==3){
                printf(" B |");
            }
            else if (board[i][j]==4){
                printf(" C |");
            }
        }
        printf("\n");
    }
    printf("  ---------------------------------\n");
    printf("    a   b   c   d   e   f   g   h\n");
}
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
    int a=*pawns;
    int b=*kings;
    int p = board[x1][y1];
    if (board[x1][y1]==1 || board[x1][y1]==2){
        int x3=(x1+x2)/2;
        int y3=(y1+y2)/2;
        if (board[x3][y3]==1 || board[x3][y3]==2){
            a--;
            *pawns=a;
        }
        else{
            b--;
            *kings=b;
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
                        a--;
                        *pawns=a;
                    }
                    else{
                        b--;
                        *kings=b;
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
                        a--;
                        *pawns=a;
                    }
                    else{
                        b--;
                        *kings=b;
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
                        a--;
                        *pawns=a;
                    }
                    else{
                        b--;
                        *kings=b;
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
                        a--;
                        *pawns=a;
                    }
                    else{
                        b--;
                        *kings=b;
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
// ruch bialych
void white_move(int *pawns, int *kings){
    int a=*pawns;
    int b=*kings;
    printf("ruch bialych\n");
        if (can_any_white_jump(board)==1){
            printf("Podaj wspolrzedne piona ktorego chcesz przesunac: ");
            scanf(" %c%d", &x_1_char, &x_1);
            y_1 = char_to_index(x_1_char);
            printf("Podaj wspolrzedne pola na ktore chcesz go przesunac "); 
            scanf(" %c%d", &x_2_char, &x_2);
            y_2 = char_to_index(x_2_char);
            while (can_jump_white(board,x_1,y_1,x_2,y_2)==0){
                printf("Podaj wspolrzedne piona ktorego chcesz przesunac: ");
                scanf(" %c%d", &x_1_char, &x_1);
                y_1 = char_to_index(x_1_char);
                printf("Podaj wspolrzedne pola na ktore chcesz go przesunac "); 
                scanf(" %c%d", &x_2_char, &x_2);
                y_2 = char_to_index(x_2_char);
            }
            jump(board,x_1,y_1,x_2,y_2,&black_pawns,&black_kings);
            for (int i=0;i<8;i++){
                if (board[7][i]==1){
                    board[7][i]=3;
                    a--;
                    b++;
                    *pawns=a;
                    *kings=b;
                }
            }
            show_board(board);
            x_1=x_2;
            y_1=y_2;
            while (can_jump_white(board,x_1,y_1,x_1+2,y_1+2)==1 || can_jump_white(board,x_1,y_1,x_1+2,y_1-2)==1 || can_jump_white(board,x_1,y_1,x_1-2,y_1-2)==1 || can_jump_white(board,x_1,y_1,x_1-2,y_1+2)==1){
                printf("podaj wspolrzedne pola, na ktore chcesz bic dalej");
                scanf(" %c%d", &x_2_char, &x_2);
                y_2 = char_to_index(x_2_char);
                if (can_jump_white(board,x_1,y_1,x_2,y_2)==1){
                    jump(board,x_1,y_1,x_2,y_2,&black_pawns,&black_kings);
                    for (int i=0;i<8;i++){
                        if (board[7][i]==1){
                            board[7][i]=3;
                            a--;
                            b++;
                            *pawns=a;
                            *kings=b;
                        }
                    }
                    show_board(board);
                    x_1=x_2;
                    y_1=y_2;
                }
                }
        }
        else{
            while (1){
                printf("Podaj wspolrzedne piona ktorego chcesz przesunac: ");
                scanf(" %c%d", &x_1_char, &x_1);
                y_1 = char_to_index(x_1_char);
                printf("Podaj wspolrzedne pola na ktore chcesz go przesunac "); 
                scanf(" %c%d", &x_2_char, &x_2);
                y_2 = char_to_index(x_2_char);
                if (can_move_white(board,x_1,y_1,x_2,y_2)==1){
                    move_pawn(board,x_1,y_1,x_2,y_2);
                    for (int i=0;i<8;i++){
                        if (board[7][i]==1){
                            board[7][i]=3;
                            a--;
                            b++;
                            *pawns=a;
                            *kings=b;
                        }
                    }
                    show_board(board);
                    break;
                }
            }
        }
}
// ruch czarnych
void black_move(int *pawns, int *kings){
    int a=*pawns;
    int b=*kings;
    printf("ruch czarnych\n");
        if (can_any_black_jump(board)==1){
            printf("Podaj wspolrzedne piona ktorego chcesz przesunac: ");
            scanf(" %c%d", &x_1_char, &x_1);
            y_1 = char_to_index(x_1_char);
            printf("Podaj wspolrzedne pola na ktore chcesz go przesunac "); 
            scanf(" %c%d", &x_2_char, &x_2);
            y_2 = char_to_index(x_2_char);
            while (can_jump_black(board,x_1,y_1,x_2,y_2)==0){
                printf("Podaj wspolrzedne piona ktorego chcesz przesunac: ");
                scanf(" %c%d", &x_1_char, &x_1);
                y_1 = char_to_index(x_1_char);
                printf("Podaj wspolrzedne pola na ktore chcesz go przesunac "); 
                scanf(" %c%d", &x_2_char, &x_2);
                y_2 = char_to_index(x_2_char);
            }
            jump(board,x_1,y_1,x_2,y_2,&white_pawns,&white_kings);
            for (int i=0;i<8;i++){
                if (board[0][i]==2){
                    board[0][i]=4;
                    a--;
                    b++;
                    *pawns=a;
                    *kings=b;
                }
            }
            show_board(board);
            x_1=x_2;
            y_1=y_2;
            while (can_jump_black(board,x_1,y_1,x_1+2,y_1+2)==1 || can_jump_black(board,x_1,y_1,x_1+2,y_1-2)==1 || can_jump_black(board,x_1,y_1,x_1-2,y_1-2)==1 || can_jump_black(board,x_1,y_1,x_1-2,y_1+2)==1){
                printf("podaj wspolrzedne pola, na ktore chcesz bic dalej");
                scanf(" %c%d", &x_2_char, &x_2);
                y_2 = char_to_index(x_2_char);
                if (can_jump_black(board,x_1,y_1,x_2,y_2)==1){
                    jump(board,x_1,y_1,x_2,y_2,&white_pawns,&white_kings);
                    for (int i=0;i<8;i++){
                        if (board[0][i]==2){
                            board[0][i]=4;
                            a--;
                            b++;
                            *pawns=a;
                            *kings=b;
                        }
                    }
                    show_board(board);
                    x_1=x_2;
                    y_1=y_2;
                }
                }
        }
        else{
            while (1){
                printf("Podaj wspolrzedne piona ktorego chcesz przesunac: ");
                scanf(" %c%d", &x_1_char, &x_1);
                y_1 = char_to_index(x_1_char);
                printf("Podaj wspolrzedne pola na ktore chcesz go przesunac "); 
                scanf(" %c%d", &x_2_char, &x_2);
                y_2 = char_to_index(x_2_char);
                if (can_move_black(board,x_1,y_1,x_2,y_2)==1){
                    move_pawn(board,x_1,y_1,x_2,y_2);
                    for (int i=0;i<8;i++){
                        if (board[0][i]==2){
                            board[0][i]=4;
                            a--;
                            b++;
                            *pawns=a;
                            *kings=b;
                        }
                    }
                    show_board(board);
                    break;
                }
            }
        }
}

int main(){
    printf("Oznaczenia: b-bialy pion, c-czarny pion, B-biala damka, C-czarna damka, . -puste czarne pole\n");
    show_board(board);
    while ((white_pawns+white_kings)>0 && (black_pawns+black_kings)>0){
        white_move(&white_pawns,&white_kings);
        // show_board(board);
        if (((white_pawns+white_kings)==0) || ((black_pawns+black_kings)==0)){
            break;
        }
        black_move(&black_pawns,&black_kings);
        // show_board(board);
    }
    game_over = 1;
    printf("Koniec gry!\n");        
    return 0;
}