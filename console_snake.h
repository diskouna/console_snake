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


/*      Control keys         */
#define CTRL_UP             '8'
#define CTRL_DOWN           '2'
#define CTRL_RIGHT          '6'
#define CTRL_LEFT           '4'

#define CTRL_PAUSE_RESUME   '0'
#define CTRL_QUIT           '5'

// TODO : ADD play again button

void print_usage(void);

struct termios  saved_attributes = {0};

void disable_input_buffering(void);
void restore_terminal_attr(int signal);
bool is_key_pressed(void);

/*
 *   QUEUE 
 **/

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

struct node * make_node(int x, int y);
void init_queue(Queue *q);
bool is_queue_empty(Queue q);
void push_back(Queue *q, int x, int y);
void pop_back(Queue *q);
void push_front(Queue *q, int x, int y);
void pop_front(Queue *q);
void delete_queue(Queue *q);



#define MAX_WIDTH  20
#define MAX_HEIGHT 10

char board[MAX_HEIGHT * MAX_WIDTH];
#define CELL_AT(x, y) board[(y) * width + (x)]

size_t width;
size_t height;

#define SNAKE_ASCII  '#'
#define APPLE_ASCII  '@'
#define EMPTY_ASCII  '.'


/*
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


#define DIR_COUNT 4   // Directions count

int dx[] = {    -1,        1,      0,      0};
int dy[] = {     0,        0,     -1,      1};
//              LEFT     RIGHT     UP     DOWN   
int dir_x;
int dir_y;

typedef enum {
    MOVE_INVALID,

    MOVE_WIN,
} move_status_t;

void init_board(void);
bool generate_apple(void);
void render_board(void);
bool is_valid_move(int next_x, int next_y);
bool move_snake_by(int dx, int dy);
bool move_right(void);
bool move_up(void);
bool move_left(void);
bool move_down(void);

#endif //__CONSOLE_SNAKE__