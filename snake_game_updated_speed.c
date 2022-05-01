#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
//Initialize Interior Borders for Snake Starting Location
#define V_Border 60
#define H_Border 200
//Ascii Character Numbers
#define down 115
#define up 119
#define left 97
#define right 100
//Using characters W,A,S,D , respectively, up, left, down, right
//define snake structure
typedef struct Snake
{
    char symbol;
    int size;
    char direction;
    char prev_direction;
    int tail_X;
    int tail_Y;
    int head_X;
    int head_Y;
}snake;
//define snake position 
typedef struct snake_pos
{
    int Y[V_Border*H_Border];
    int X[V_Border*H_Border];
}snake_pos;
//define treasure structure
typedef struct Treasure
{
    int treasure;
    int prev_X;
    int prev_Y;
    int X;
    int Y;
} treasure;
//Initialize snake
void snake_init(snake *snake);
//Initialize snake position
void init_pos(snake_pos *pos);
//Move cursor function prototype
void move_cur(int,int);
//Snake location function prototype
void snake_loc(snake *snake, snake_pos *pos);
//Snake moving function prototype
void snake_move(snake *snake, snake_pos *pos, treasure *treasure);
//Snake tail updating to screen prototype
void move_tail(snake *snake, snake_pos *pos);
//Snake head updating to screen prototype
void move_head(snake *snake, snake_pos *pos);
//Game over when snake hits borders protoype
int over(snake *snake, snake_pos *pos);
//Treasure placement Signal Handler Protoype
void treasure_placement(int);
//Displays score in bottom left corner of screen
void display_score(snake *snake);
int keyboard_hit(void);
void borders();
time_t t;
//Global so that SIGALRM can access
treasure treasure1;
snake snake1;
snake_pos pos1;
//Snake moving speed
int snake_speed=90500;
int main()
{
    //Snake moving speed
      int snake_speed=90500;
      srand((unsigned) time (&t));
    
    //Initialize screen, snake, set cursor visibility to 0 and initial position of snake
      
      
      signal(SIGALRM,treasure_placement);
      alarm(1);
      
      snake_init(&snake1);
      init_pos(&pos1);
      initscr();
      clear();
      noecho();
       
      keypad(stdscr,TRUE);
      curs_set(0);    
      snake_loc(&snake1,&pos1);
      borders();
    //Pipe keyboard into program without buffer
      raw();
        display_score(&snake1);
    //Runs in one direction until snake hits border
     
      while(!(over(&snake1,&pos1)))
      {
    //While no key pressed move snake in current direction, update speed of snake based on size of snake
    //(Jon Ventres)
          while (!keyboard_hit())
          {
              if(snake1.size ==3){
                   usleep(snake_speed);
              }
              else if(snake1.size<=10 && snake1.size>3){
                  usleep(89000);
              }else if(snake1.size<=15 && snake1.size>10){
                  usleep(87000);
              }else if(snake1.size<20 && snake1.size>15){
                  usleep(85000);
              }else{
                  usleep(84000);
              }
                 
                 snake_move(&snake1,&pos1,&treasure1);
                 if (over(&snake1,&pos1))
                 {
                     break;
                 }
          }
         //Add previous direction to snake direction and get new one
          snake1.prev_direction=snake1.direction;
          snake1.direction=getchar();
     }
      // Refresh the screen on termination
      refresh();
      endwin();
      return 0;
}
//Mike and Jon: Initializes the borders for the program (first and last column, first and last row, filled with *'s)
void borders()
{
    for (int i=0; i<LINES-1;i++)
    {
        move_cur(0,i);
        printf("*");
 
        move_cur(COLS-1,i);
        printf("*");
    }
    for (int i=0; i<COLS-1; i++)
        {
        move_cur(i,0);
        printf("*");
        move_cur(i,LINES-1);
        printf("*");
        }
}
//Initialize the snake  with a size of 3 and a random direction. (Dylan Fassio)
void snake_init(snake *snake1)
{
    srand(time(0));
    int init_dir = rand() % 4;
    snake1->symbol = 'X';
    snake1->size = 3;
    snake1->tail_X = V_Border/2;
    snake1->tail_Y = H_Border/2;
    switch (init_dir){
        case 0:
            snake1->direction = right;
            snake1->head_X = snake1->tail_Y + snake1->size - 1;
            snake1->head_Y = V_Border/2;
            break;
        case 1:
            snake1->direction = up;
            snake1->head_X = H_Border/2;
            snake1->head_Y = snake1->tail_X + snake1->size - 1;
            break;
        case 2:
            snake1->direction = left;
            snake1->head_X = snake1->tail_Y - snake1->size -1;
            snake1->head_Y = V_Border/2;
            break;
        case 3:
            snake1->direction = down;
            snake1->head_X = H_Border/2;
            snake1->head_Y = snake1->tail_X - snake1->size - 1;
            break;
        default:
            break;
    }
}
//Jon Ventres: Setup terminal connection attributes, treating keyboard inputs. Returns 1 if keys are hit, 0 if not.
int keyboard_hit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
//Get parameters associated with terminal
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
//Change flags of Canonical and Echo modes
  newt.c_lflag &= ~(ICANON | ECHO);
//Set terminal attributes immediately to newt
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
//Oldf = manipulate standard in, return flags
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
//Set file descriptor flags to the value of oldf | non blocking mode
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
//Set char = next input
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
//If character is at end of file return 1 else return 0
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
  
  return 0;
}
//Jon Ventres: moves the snake to the cursor position
void snake_loc(snake *snake1, snake_pos *pos1)
{
    for (int i=0; i<snake1->size; ++i)
    {
        move_cur(snake1->tail_X,snake1->tail_Y);
        printf("%c",snake1->symbol);
        pos1->X[i]=snake1->tail_X;
        pos1->Y[i]=snake1->tail_Y;
        snake1->tail_X+=1;
    }
}
//Mike and Jon: Moves the head and tail of the snake. used in the main method.
void snake_move(snake *snake1, snake_pos *pos1, treasure *treasure1)
{
    move_head(snake1,pos1);
    move_tail(snake1,pos1);
    //If snake hits treasure, grows by number of treasure (Mike Ronalter)
    if((snake1->head_X == treasure1->X) && (snake1->head_Y == treasure1->Y))
    {
            //Cancels current alarm, restarts another at 1 second
            alarm(0);   
            alarm(1);
                  
            snake1->size = snake1->size + treasure1->treasure;
            
    
            move_head(snake1, pos1);
            move_tail(snake1, pos1);
    
            display_score(snake1);
   
        }
}
//Jon Ventres: moves the tail by printing " " at the end of the snake
void move_tail(snake *snake1, snake_pos *pos1)
{
    //remove last cell of tail
    move_cur(pos1->X[0],pos1->Y[0]);
    printf(" ");
    //update new tail position
    for (int i=0; i<snake1->size; ++i)
    {
        pos1->X[i]=pos1->X[i+1];
        pos1->Y[i]=pos1->Y[i+1];
    }
}
//Jon Ventres: moves the head in the direction specified by main method. if the user trues going in the opposite direction, it will end the game
void move_head(snake *snake, snake_pos *pos)
{
//Snake direction based off defined ascii codes for "w,a,s,d" 
    switch (snake->direction)
        {
            //If character "w" entered and snake is moving down add y coordinate head++ else decrease head--
            case up:
                if (snake->direction==down)
                {
                    snake->head_Y++;
                    break;
                }
                    snake->head_Y--;
                    break;
            
            //If character "s" entered and snake is moving up decrease y coordinate head-- else increase head++
            case down:
                if (snake->direction==up)
                {
                    snake->head_Y--;
                    break;
                }
                    snake->head_Y++;
                    break;
            //If character "d" entered and snake is moving left decrease x coordinate head-- else increase head++
            case right:
                if (snake->direction==left)
                {
                    snake->head_X--;
                    break;
                }
                    snake->head_X++;
                    break;
            //If character "a" entered and snake is moving right add x coordinate head++ else decrease head--
            case left:
                if (snake->direction==right)
                {
                    snake->head_X++;
                    break;
                }
                    snake->head_X--;
                    break;
            default:
                 break;
        }
        pos->X[snake->size]=snake->head_X;
        pos->Y[snake->size]=snake->head_Y;
        move_cur(pos->X[snake->size],pos->Y[snake->size]);
        printf("%c",snake->symbol);        
}
//Jon Ventres: Set cursor to x,y and esc print
void move_cur(int x,int y)
{
    printf("\e[%d;%df",y,x);
}
//Jon Ventres: Copy the 0 unsigned char to sizeof onto pointer position
void init_pos(snake_pos *position)
{
    memset(position, 0, sizeof(*position));
}
//Jon and Dylan: Checks to see whether the snake has reached the borders, if the snake has crashed into itself, or if player wins
int over(snake *snake1, snake_pos *pos1)
{
    for (int i=0; i<snake1->size-1; ++i)
    {
        if ((pos1->X[i]==snake1->head_X) && (pos1->Y[i]==snake1->head_Y))
        {
            return 1;
        }
    }
    int horiz = COLS-1;
    int vert = LINES-1;
    //Check to see if the snake head has reached either horizontal or vertical border
    if ((snake1->head_X==horiz) || (snake1->head_X==1) || (snake1->head_Y==vert) || (snake1->head_Y==1))
        {
            return 1;
        }

    if (snake1->size == 10)
    if (snake1->size == (V_Border + H_Border))
        return 1;

    return 0;
}
//Places treasure randomly on board within borders, if snake does not get to in time, randomly moves to another space
//Treasure randomly ranges between 1 and 9 and lasts # of seconds depending on treasure value (IE. 1 lasts 27 secs, 2 - 24 secs) (Mike Ronalter)
void treasure_placement(int signum)
{
       
     //Calls treasure_placement function when alarm finishes
     signal(SIGALRM, treasure_placement);
      
     int x, y, interval;
        //Does not fire if first time as there is no true previous x or previous y for treasure
        if(treasure1.prev_X != -1 || treasure1.prev_Y != -1)
        {
            move_cur(treasure1.prev_X,treasure1.prev_Y);
            printf(" ");
        
        }
    
    interval = (rand() % 9) + 1; //1 - 9
    treasure1.treasure = interval;
    switch(interval){
        case 1:
            alarm(27);
            break;
        case 2:
            alarm(24);
            break;
        case 3:
            alarm(21);
            break;
        case 4:
            alarm(18);
            break;
        case 5:
            alarm(15);
            break;
        case 6:
            alarm(12);
            break;
        case 7:
            alarm(9);
            break;
        case 8:
            alarm(6);
            break;
        case 9:
            alarm(3);
            break;
        
    }
    x = (rand() % (COLS-1)); 
    //keeps within screen
    if(x<2)
        x=2;
    treasure1.X = x;
   
    y = (rand() % (LINES-1)); 
    //keeps within screen
    if(y<2)
        y=2;
   
    treasure1.Y = y;
    //Placement of Treasure
    move_cur(treasure1.X,treasure1.Y);
    printf("%d",interval);
    
    treasure1.prev_X = x;
    treasure1.prev_Y = y;
}
//prints score in bottom left corner of screen (Dylan Fassio)
void display_score(snake *snake){
    move_cur(0, COLS-1);
    printf("Your score is: %d/%d", (snake->size -3), (COLS -1 + LINES -1));
}
//Charles Lewis: Finished working code and did some minor bug fixes(User being able to press opposite direction arrow twice before game over was detected).
//Mike Ronalter: Created border and intial setup of snake on screen moving to the right with size 3.
//Jonathan Ventres: Created snake moving up, down, side to side, modified keyboard input, modified gotoxy and speed based on size.
//Dylan Fassio: Worked on project framework, initializing snake, implementing snake movement logic.
