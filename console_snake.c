
#include "console_snake.h"

Queue snake;

size_t score = 0;
size_t apple_count = 0;

int main() 
{
    disable_input_buffering();
    signal(SIGINT, &restore_terminal_attr); // Handle SIGINT interrupt, 
                                            // sent by  "CTRL + C" for example
    init_board();
    bool close = false;
    bool pause = false;
    while (!close) {         
        render_board();
        if (is_key_pressed()) {
            char pressed_key = getchar();
            switch (pressed_key) {
                case CTRL_PAUSE_RESUME : {
                            pause = ~pause;
                            if (pause) {
                                while (!is_key_pressed() || getchar() != CTRL_PAUSE_RESUME)
                                    ;
                                printf("Pause");
                            }
                            break;
                        }
                case CTRL_QUIT : {
                            close = true;
                            break;
                        }
                case CTRL_RIGHT : {
                            if (!move_right()) {
                                printf("You lose\n");
                                close = true;
                            }
                            break;    
                        }
                case CTRL_UP : {
                            if (!move_up()) {
                            printf("You lose\n");
                            close = true;
                        }
                        break;
                        }
                case CTRL_LEFT : {
                            if (!move_left()) {
                                printf("You lose\n");
                                close = true;
                            }
                            break;
                        }
                case CTRL_DOWN : {
                            if (!move_down()) {
                                printf("You lose\n");
                                close = true;
                            }
                            break;
                        }   
                default : {
                            render_board();
                            printf("Invalid entry\n");
                        }     
            }
        } else {
            // keep moving according to the last set direction
            if (!move_snake_by(dir_x, dir_y)) {
                printf("You lose\n");
                close = true;
            }
        }
    }

    restore_terminal_attr(0);

    return 0;
}

void print_usage(void)
{
    printf("Press '%c' to move up \n", CTRL_UP);
    printf("Press '%c' to move down \n", CTRL_DOWN);
    printf("Press '%c' to move left \n", CTRL_LEFT);
    printf("Press '%c' to move right \n", CTRL_RIGHT);

    printf("Press '%c' to pause/resume \n", CTRL_PAUSE_RESUME);
    printf("Press '%c' to quit \n",        CTRL_QUIT);
}

/*                Board                  */

void init_board(void) 
{
    srand(time(NULL));    

    width  = MAX_WIDTH;
    height = MAX_HEIGHT; 
    for (size_t y = 0; y < height && y < MAX_HEIGHT; y++) 
        for (size_t x = 0; x < width && x < MAX_WIDTH; x++)        
            CELL_AT(x, y) = EMPTY_ASCII;

    // init snake position 
    push_front(&snake, /*rand() % width*/0, /*rand() % height*/0);
     
    generate_apple();

    // init direction
    int dir = rand() % DIR_COUNT;
    dir_x = dx[dir];  
    dir_y = dy[dir];
}

// TODO : optimize the generation
bool generate_apple(void) 
{
    if (apple_count == 0) {
        size_t apple_pos; 
        int try_cnt = 5;   
        do {
            apple_pos = rand() % (width * height);
            try_cnt--;
        } while (try_cnt  && board[apple_pos] == SNAKE_ASCII);
        
        if (try_cnt != 0) {
            board[apple_pos] = APPLE_ASCII;
            apple_count = 1;
            return true;
        }
    }
    return false;
}

void render_board(void)
{
    // ANSI escape sequence for clearing the screen
    printf("\033[2J"); 
    // ANSI escape sequence for positioning the cursor at the beginning 
    printf("\033[H"); 

    printf("score = %5zu \n", score);
    printf("====================\n");
    generate_apple();
    for (size_t y = 0; y < height && y < MAX_HEIGHT; y++) {
        for (size_t x = 0; x < width && y < MAX_WIDTH; x++) 
            putchar(CELL_AT(x, y));
        putchar('\n');
    }
    printf("\n====================\n");
    print_usage();
    fflush(stdout);
} 

bool is_valid_move(int next_x, int next_y)
{
    return (0 <= next_x  && (size_t)next_x < width  && 
            0 <= next_y && (size_t)next_y < height  && 
            CELL_AT(next_x, next_y) !=  SNAKE_ASCII   ); 
}

bool move_snake_by(int dx, int dy)
{  
    // save the set direction
    dir_x        = dx ;
    dir_y        = dy ;
    if (is_valid_move(snake.head->x + dx, snake.head->y + dy)) {   

        if (CELL_AT(snake.head->x + dx, snake.head->y +dy) == APPLE_ASCII) {
            // update score
            score++; 
            apple_count = 0;
            // update snake's head
            push_front(&snake, snake.head->x + dx, snake.head->y + dy);
            CELL_AT(snake.head->x, snake.head->y) = SNAKE_ASCII;
        } else {
            // update snake's head
            push_front(&snake, snake.head->x + dx, snake.head->y + dy);
            CELL_AT(snake.head->x, snake.head->y) = SNAKE_ASCII;
            // update snake's tail
            CELL_AT(snake.tail->x, snake.tail->y) = EMPTY_ASCII;
            pop_back(&snake);
        }

        render_board();

        if (snake.size == width * height) {
            printf("You win \n");
            // return enum 
        } 

        return true;
    } 
    return false;
}

bool move_right(void)
{
    return move_snake_by(+1, 0);
}

bool move_up(void)
{
    return move_snake_by(0, -1);
}

bool move_left(void)
{
    return move_snake_by(-1, 0);
}

bool move_down(void)
{
    return move_snake_by(0, +1);
}


/*                Queue                  */


struct node * make_node(int x, int y)
{
    struct node *n = (struct node*)malloc(sizeof(*n));
    if (n == NULL) 
        exit(EXIT_FAILURE);
    n->x = x;
    n->y = y;
    n->next = n->previous = NULL;
    return n;
}

void init_queue(Queue *q)
{
    q->head = q->tail = NULL;
    q->size = 0;
}

bool is_queue_empty(Queue q)
{
    return (q.size == 0);
}

void push_back(Queue *q, int x, int y)
{   
    struct node *n = make_node(x, y);
    if (is_queue_empty(*q)) {
        q->head = q->tail = n;
    } else {
        q->tail->next = n;
        n->previous = q->tail;
        q->tail = n;
    }
    q->size++;
}

void pop_back(Queue *q)
{
    if (!is_queue_empty(*q)) {
        struct node *tmp = q->tail->previous;
        free(q->tail);
        if (tmp == NULL) 
            q->head = NULL;
        else 
            tmp->next = NULL;
        q->tail = tmp;
        q->size--;
    }
}

void push_front(Queue *q, int x, int y)
{
    struct node *n = make_node(x, y);
    if (is_queue_empty(*q)) {
        q->head = q->tail = n;
    } else {
        q->head->previous = n;
        n->next = q->head;
        q->head = n;
    }
    q->size++;
}

void pop_front(Queue *q)
{
    if (!is_queue_empty(*q)) {
        struct node *tmp = q->head->next;
        free(q->head);
        if (tmp == NULL)
            q->tail = NULL;
        else
            tmp->previous = NULL;
        q->head = tmp;   
        q->size--; 
    }
}

void delete_queue(Queue *q)
{
    while (!is_queue_empty(*q))
        pop_back(q);    
}

/*        TERMINAL         */

void disable_input_buffering(void)
{
    int res = tcgetattr(STDIN_FILENO, &saved_attributes);
    if (res == -1) { 
       perror("tcgetattr : ");
       exit(1);
    }
    struct termios new_attributes = saved_attributes;
    
    // disable the echo and canon 

    new_attributes.c_lflag &= ~(ECHO | ICANON);
    res = tcsetattr(STDIN_FILENO, TCSANOW, &new_attributes); 
    if (res == -1) {
       perror("tcsetattr : ");
       exit(1);
    }
}

void restore_terminal_attr(int signal) 
{
    (void) signal;
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);   
    delete_queue(&snake);
    exit(0);
}

/* wait for maximum 1 sec until a key is pressed
 * return true when a key was pressed and
 *        false otherwise
 */
bool is_key_pressed(void)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
    return (select(STDIN_FILENO+1, &readfds, NULL, NULL, &timeout) == 1);
}
