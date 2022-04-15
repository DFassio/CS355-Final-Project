#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

#define V_Border 80
#define H_Border 300
#define down 115
#define up 119
#define left 97
#define right 100

//Using characters W,A,S,D

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


void snake_init(snake *snake1);
void pos_init(snake_pos *pos1);
void move_cur(int,int);
void snake_loc(snake *snake1, snake_pos *pos1);
void snake_move(snake *snake1, snake_pos *pos1);
void move_tail(snake *snake1, snake_pos *pos1);
void move_head(snake *snake1, snake_pos *pos1);
int game_over(snake *snake1, snake_pos *pos1);
void borders();



int main()
{
    //Snake moving speed
      int snake_speed=90500;
    
    //Initialize screen and snake

      snake snake1;
      snake_pos pos1;
      snake_init(&snake1);
      pos_init(&pos1);

      initscr();
      clear();
      noecho();
      curs_set(0);      
      snake_loc(&snake1,&pos1);
      borders();
    //Pipe keyboard into program without buffer
      raw();


    //Runs until snake hits border 
     
      while(!(game_over(&snake1,&pos1)))
      {

        //While no key pressed move snake in current direction

          while (!keyboard_hit())
          {
                 usleep(snake_speed);
                 snake_move(&snake1,&pos1);
                 if (game_over(&snake1,&pos1))
                 {
                     break;
                 }

          }
         //Add previous direction to direction and get new one

          snake1.prev_direction=snake1.direction;
          snake1.direction=getchar();

     }
      // Refresh the screen on termination
      refresh();
      endwin();

      return 0;
}

//Initialize the snake 
void snake_init(snake *snake1)
{
    snake1->symbol='-';
    snake1->size=3;
    snake1->direction=right;
    snake1->prev_direction=down;
    snake1->tail_X=5;
    snake1->tail_Y=5;
    snake1->head_X=snake1->tail_X+snake1->size-1;
    snake1->head_Y=7;
}


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

void borders()
{
    int i;
    for (i=0; i<V_Border; ++i)
    {
        move_cur(0,i);
        printf("X");
 
        move_cur(H_Border,i);
        printf("X");
    }

    for (i=0; i<H_Border; ++i)
        {
        move_cur(i,0);
        printf("X");
        move_cur(i,V_Border);
        printf("X");
        }
}



void snake_move(snake *snake1, snake_pos *pos1)
{
    move_head(snake1,pos1);
    move_tail(snake1,pos1);
    
}



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



void move_head(snake *snake1, snake_pos *pos1)
{
    switch (snake1->direction)
        {
            case right:
                if (snake1->prev_direction==left)
                {
                    snake1->head_X--;
                    break;
                }
                    snake1->head_X++;
                    break;

            case left:
                if (snake1->prev_direction==right)
                {
                    snake1->head_X++;
                    break;
                }
                    snake1->head_X--;
                    break;


            case up:
                if (snake1->prev_direction==down)
                {
                    snake1->head_Y++;
                    break;
                }
                    snake1->head_Y--;
                    break;


            case down:
                if (snake1->prev_direction==up)
                {
                    snake1->head_Y--;
                    break;
                }
                    snake1->head_Y++;
                    break;


            default:
                 break;
        }


        // update tail position
        pos1->X[snake1->size]=snake1->head_X;
        pos1->Y[snake1->size]=snake1->head_Y;

        move_cur(pos1->X[snake1->size],pos1->Y[snake1->size]);
        printf("%c",snake1->symbol);
}


//Move Cursor and print 
void move_cur(int x,int y)
{
    printf("\e[%d;%df",y,x);
}




void pos_init(snake_pos *pos1)
{
    memset(pos1, 0, sizeof(*pos1));
}


int game_over(snake *snake1, snake_pos *pos1)
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



int keyboard_hit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}
