#ifndef __CONSOLE_SNAKE__
#define __CONSOLE_SNAKE__
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

// POSIX headers
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>


/*      Control keys          */
#define CTRL_UP             'k'
#define CTRL_DOWN           'j'
#define CTRL_RIGHT          'l'
#define CTRL_LEFT           'h'

#define CTRL_PAUSE_RESUME   'p'
#define CTRL_QUIT           'q'
// TODO : ADD play again button

/*           BOARD 
 * x is the horizontal's axe
 * y is the descending vertical's axe
 * 
 *  ------------------------------> x
 *  |                           |
 *  |                           |
 *  |                           | h
 *  |                           | e
 *  |                           | i
 *  |                           | g
 *  |                           | h
 *  |                           | t
 *  |                           |
 *  |___________________________|
 *  |            width
 *  V y
 *  
 */ 

/*    BOARD "Matrix" interface  */
#define MAX_WIDTH  25
#define MAX_HEIGHT 10
size_t width;
size_t height;
char board[MAX_HEIGHT * MAX_WIDTH];
#define CELL_AT(x, y) board[(y) * width + (x)]

/* Game rendering ASCII characters*/

#define ASCII_RENDER_SNAKE  '#'
#define ASCII_RENDER_APPLE  '@'
#define ASCII_RENDER_EMPTY  '.'

/* ANSI escape sequences for colored printing*/

#define RESET_COLOR     "\033[m"
#define IN_RED_COLOR    "\033[38;5;9m"
#define IN_GREEN_COLOR  "\033[38;5;10m"

/*     Game rendering functions     */
void init_rendering(void);
void emergency_close(int signal);
void close_rendering(void);
void init_game(void);
void close_game(void);
void render_game(void);
void init_board(void);
void render_board(void);
void init_snake_position(void);
void init_snake_direction(void);
bool generate_apple(void);
void print_usage(void);

/*        Directions         */
#define DIR_COUNT 4   // Directions count

int dx[] = {    -1,        1,      0,      0};
int dy[] = {     0,        0,     -1,      1};
//              LEFT     RIGHT     UP     DOWN 
int dir_x;
int dir_y;

/*  Snake state definition     */
typedef enum {
    SNAKE_DEAD = 0,
    SNAKE_LIVE,
    SNAKE_FULL,
} snake_state_t;

/*       Snake move functions     */
bool is_safe_move(int next_x, int next_y);
snake_state_t move_snake_by(int dx, int dy);
snake_state_t move_right(void);
snake_state_t move_up(void);
snake_state_t move_left(void);
snake_state_t move_down(void);

/*   Queue definition   */
struct node {
    int x;
    int y;
    struct node *next;
    struct node *previous;
};

typedef struct queue {
    struct node *head;
    struct node *tail;
    size_t size;
} Queue;

/*          Queue functions               */
struct node * make_node(int x, int y);
void init_queue(Queue *q);
bool is_queue_empty(Queue q);
void push_back(Queue *q, int x, int y);
void pop_back(Queue *q);
void push_front(Queue *q, int x, int y);
void pop_front(Queue *q);
void delete_queue(Queue *q);

/*        Terminal input functions         */
void init_terminal(void);
void restore_terminal(void);
bool is_key_pressed(void);

#endif //__CONSOLE_SNAKE__