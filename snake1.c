#include <stdio.h>
#include <curses.h>
#include <unistd.h>

void start_game();
//movement functions
void move_right();
void move_left();
void move_up();
void move_down();

//x and y values point to the back of the snake
int x = 0; //represents COLS (x-axis)
int y = 0; //represents LINES (y-axis)

int snakeLength = 5;

int main(){
    start_game();
    move_right();
//add switch that will take user input and call movement function based on which arrow key was pressed


    getch();
    endwin();
}

//function that creates border around terminal and creates the initial snake
void start_game(){
    initscr();
        clear();

        //create border here        

        //initialize snake in middle of terminal
        y = LINES/2;
        x = COLS/2;
        int temp = x;
        for (int i = 0; i < snakeLength; i++){   //builds snake from back to front
            move(y, ++temp);                     //using a temp variable to make sure x and y
            addstr("~");                         //still point to back of snake
        }
}

void move_right(){
    move(y, x);
    int temp = x + snakeLength;
    while(temp < COLS-1 || getch()){             //moves the snake to the right by adding 
        move(y, temp++);                         //characters to the front of the snake
        addstr("~");                             //and adding a blank space to where the 
        refresh();                               //last character of the snake was
        sleep(1);
        move(y, x);
        addstr("  ");
        x++;
    }
}