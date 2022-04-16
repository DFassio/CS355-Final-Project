#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

//Border Size Fixed instead of LINES-1, COL-1
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

//Initialize snake
void snake_init(snake *snake);
//Initialize snake position
void init_pos(snake_pos *pos);
//Move cursor function prototype
void move_cur(int,int);
//Snake location function prototype
void snake_loc(snake *snake, snake_pos *pos);
//Snake moving function prototype
void snake_move(snake *snake, snake_pos *pos);
//Snake tail updating to screen prototype
void move_tail(snake *snake, snake_pos *pos);
//Snake head updating to screen prototype
void move_head(snake *snake, snake_pos *pos);
//Game over when snake hits borders protoype
int over(snake *snake, snake_pos *pos);

int keyboard_hit(void);
void borders();



int main()
{
    //Snake moving speed
      int snake_speed=90500;
    
    //Initialize screen, snake, set cursor visibility to 0 and initial position of snake

      snake snake;
      snake_pos pos;
      snake_init(&snake);
      init_pos(&pos);
      initscr();
      clear();
      noecho();
      curs_set(0);      
      snake_loc(&snake,&pos);
      borders();
    //Pipe keyboard into program without buffer
      raw();


    //Runs in one direction until snake hits border
     
      while(!(over(&snake,&pos)))
      {

    //While no key pressed move snake in current direction

          while (!keyboard_hit())
          {
                 usleep(snake_speed);
                 snake_move(&snake,&pos);
                 if (over(&snake,&pos))
                 {
                     break;
                 }

          }
         //Add previous direction to snake direction and get new one

          snake.prev_direction=snake.direction;
          snake.direction=getchar();

     }
      // Refresh the screen on termination
      refresh();
      endwin();

      return 0;
}
//initializes the borders for the program (first and last column, first and last row, filled with *'s)
void borders()
{
    for (int i=0; i<V_Border; i++)
    {
        move_cur(0,i);
        printf("*");
 
        move_cur(H_Border,i);
        printf("*");
    }

    for (int i=0; i<H_Border; i++)
        {
        move_cur(i,0);
        printf("*");
        move_cur(i,V_Border);
        printf("*");
        }
}

//Initialize the snake  with a size of 5 and a right direction.
void snake_init(snake *snake1)
{
    snake1->symbol='_';
    snake1->size=5;
    snake1->direction=right;
    snake1->prev_direction=down;
    snake1->tail_X=LINES/2;
    snake1->tail_Y=COLS/2;
    snake1->head_X=snake1->tail_X+snake1->size-1;
    snake1->head_Y=7;
}

//Setups terminal connection attributes, treating keyboard inputs. Returns 1 if keys are hit, 0 if not.
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

//moves the snake to the cursor position
void snake_loc(snake *snake1, snake_pos *pos1)
{
    int i;
    for (i=0; i<snake1->size; ++i)
    {
        move_cur(snake1->tail_X,snake1->tail_Y);
        printf("%c",snake1->symbol);
        pos1->X[i]=snake1->tail_X;
        pos1->Y[i]=snake1->tail_Y;
        snake1->tail_X+=1;
    }

}


//Moves the head and tail of the snake. used in the main method.
void snake_move(snake *snake1, snake_pos *pos1)
{
    move_head(snake1,pos1);
    move_tail(snake1,pos1);
    
}


//moves the tail by priting " " at the end of the snake
void move_tail(snake *snake1, snake_pos *pos1)
{
    int i;

    //remove last cell of tail
    move_cur(pos1->X[0],pos1->Y[0]);
    printf(" ");


    //update new tail position
    for (i=0; i<snake1->size; ++i)
    {
        pos1->X[i]=pos1->X[i+1];
        pos1->Y[i]=pos1->Y[i+1];
    }
}


//moves the head in the direction specified by main method. if the user trues going in the opposite direction, it will end the game
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


        // update tail position
        pos->X[snake->size]=snake->head_X;
        pos->Y[snake->size]=snake->head_Y;

        move_cur(pos->X[snake->size],pos->Y[snake->size]);
        printf("%c",snake->symbol);
}


//Set cursor to x,y and esc print
void move_cur(int x,int y)
{
    printf("\e[%d;%df",y,x);
}



//Copy the 0 unsigned char to sizeof onto pointer position
void init_pos(snake_pos *position)
{
    memset(position, 0, sizeof(*position));
}

//Checks to see whether the snake has reached the borders, or if the snake has crashed into itself
int over(snake *snake1, snake_pos *pos1)
{

    for (int i=0; i<snake1->size-1; ++i)
    {
        if ((pos1->X[i]==snake1->head_X) && (pos1->Y[i]==snake1->head_Y))
        {
            return 1;
        }
    }

//Check to see if the snake head has reached either horizontal or vertical border
    if ((snake1->head_X==H_Border) || (snake1->head_X==1) || (snake1->head_Y==V_Border) || (snake1->head_Y==1))
        {
            return 1;
        }


    return 0;
}


//Charles Lewis: Finished working code and did some minor bug fixes(User being able to press opposite direction arrow twice before game over was detected).
//Mike Ronalter: Created border and intial setup of snake on screen moving to the right with size 5.
//Jonathan Ventres: Created snake moving up, down, side to side, along with checking for keyboard input.
