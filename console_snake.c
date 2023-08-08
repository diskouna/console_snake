#include "console_snake.h"

Queue snake;
size_t score;
size_t apple_count;

int main() 
{
    init_rendering();
init :    
    init_game();
    bool close = false;
    bool pause = false;
    snake_state_t state = SNAKE_LIVE;
    while (!close) {     
        render_game();
        if (is_key_pressed()) {
            char pressed_key = getchar();
            switch (pressed_key) {
                case CTRL_PAUSE_RESUME : {
                    pause = !pause;
                    if (pause) {
                        printf("Pause\n");
                        // wait for "CTRL_PAUSE_RESUME" | "CTRL_QUIT" key to be pressed
                        while (!is_key_pressed() || ((pressed_key = getchar()) != CTRL_PAUSE_RESUME && pressed_key != CTRL_QUIT))
                            ;
                        close = (pressed_key == CTRL_QUIT);
                    }
                    break;
                }
                case CTRL_QUIT : {
                    close = true;
                    break;
                }
                case CTRL_RIGHT : {
                    state = move_right();
                    break;    
                }
                case CTRL_UP : {
                    state = move_up(); 
                    break;
                }
                case CTRL_LEFT : {
                    state = move_left();
                    break;
                }
                case CTRL_DOWN : {
                    state = move_down();
                    break;
                }      
            }
        } else {
            // keep moving in the last set direction
            state = move_snake_by(dir_x, dir_y);
        }

        if (state == SNAKE_LIVE)
            continue;
        
        // handle end of the game
        if (state == SNAKE_DEAD) {
            printf(IN_RED_COLOR "Oops! You lose! Try again\n" RESET_COLOR);     
        } else if (state == SNAKE_FULL) {
            printf(IN_GREEN_COLOR "Congratulation! You win\n" RESET_COLOR);
        }
        printf("Press 'Enter key' to play again or '%c' to leave\n", CTRL_QUIT);

        char pressed_key;
        // wait for "CTRL_QUIT" or "Enter " key to be pressed
        while (!is_key_pressed() || ((pressed_key = getchar()) != CTRL_QUIT && pressed_key != '\n')) 
            ;
        if (pressed_key == '\n')
            goto init;
        // pressed_key == CTRL_QUIT    
        close = true;    
    }

    close_game();
    close_rendering();

    return 0;
}

/*     Game rendering functions     */

void init_rendering(void)
{
    init_terminal();
    // Handle SIGINT interrupt, sent by  "CTRL + C" by example
    signal(SIGINT, &emergency_close); 
}

void emergency_close(int signal)
{
    (void) signal;
    close_game();
    close_rendering();
}

void close_rendering(void)
{
    restore_terminal();
}

void init_game(void) 
{    
    // choose board dimension
    width  = MAX_WIDTH;
    height = MAX_HEIGHT;
    init_board();
    // init the random number generator seed with current time        
    srand(time(NULL));   
    init_snake_position();
    init_snake_direction();
    // init score and apple count
    score = 0;
    apple_count = 0;
    generate_apple();

}

void close_game(void)
{
    delete_queue(&snake);
}

// fill the board with "ASCII_RENDER_EMPTY" character 
void init_board(void)
{
    for (size_t y = 0; y < height; y++) 
        for (size_t x = 0; x < width; x++)        
            CELL_AT(x, y) = ASCII_RENDER_EMPTY;
}

void init_snake_position(void)
{
    // the snake must be empty
    if (!is_queue_empty(snake)) 
        delete_queue(&snake);

    int head_x = rand() % width;
    int head_y = rand() % height;

    push_front(&snake, head_x, head_y);
}

void init_snake_direction(void)
{
    // init direction
    int dir = rand() % DIR_COUNT;
    dir_x = dx[dir];  
    dir_y = dy[dir];

    // the first move should not be a losing move
    if (!is_safe_move(snake.head->x + dir_x, snake.head->y + dir_y)) {
        // choose the opposite direction
        dir_x = -dir_x;
        dir_y = -dir_y;
    }
}

void generate_apple(void) 
{
    if (apple_count == 0) {
        // Try @attemp_cnt guesses to find 
        // an empty cell for placing an apple  
        size_t apple_pos_x, apple_pos_y; 
        int attempt_cnt = 10;   
        do {
            apple_pos_x = rand() % width;
            apple_pos_y = rand() % height;
            attempt_cnt--;
        } while (attempt_cnt > 0 && CELL_AT(apple_pos_x, apple_pos_y) == ASCII_RENDER_SNAKE);
        
        if (attempt_cnt != 0) {
            /*We found an empty space*/
            CELL_AT(apple_pos_x, apple_pos_y) = ASCII_RENDER_APPLE;
        } else { 
            /* We fail to correctly guess*/
            // We traverse the whole board to find a random empty cell
            size_t empty_cells_count = (width * height) - snake.size;
            size_t empty_cell_pos = rand() % empty_cells_count + 1;

            for (size_t y = 0; y < height; y++) {
                for (size_t x = 0; x < width; x++) {
                    if (CELL_AT(x, y) == ASCII_RENDER_EMPTY) empty_cell_pos--;
                    if (empty_cell_pos == 0) {
                        CELL_AT(x, y) = ASCII_RENDER_APPLE;
                        break;
                    }
                } 
            }
        }
        apple_count = 1;
    }
}

void render_game(void)
{
    // reset the terminal using ANSI escape sequence "\033c"
    printf("\033c");
    fflush(stdout);

    printf("score:%20zu\n", score);
    printf("===========================\n");
    generate_apple(); 
    render_board();
    print_usage(); 
} 

void render_board(void)
{
    for (size_t y = 0; y < height && y < MAX_HEIGHT; y++) {
        putchar(' ');
        for (size_t x = 0; x < width && x < MAX_WIDTH; x++) 
            putchar(CELL_AT(x, y));
        putchar('\n');
    }
}

void print_usage(void)
{
    printf("\n===========================\n");
    printf("          USAGE\n");
    printf("Press '%c' to move up\n", CTRL_UP);
    printf("Press '%c' to move down\n", CTRL_DOWN);
    printf("Press '%c' to move left\n", CTRL_LEFT);
    printf("Press '%c' to move right\n", CTRL_RIGHT);

    printf("Press '%c' to pause/resume\n", CTRL_PAUSE_RESUME);
    printf("Press '%c' to quit\n",        CTRL_QUIT);
    printf("===========================\n");
}

/*       Snake move functions      */

bool is_safe_move(int next_x, int next_y)
{
    return (0 <= next_x && (size_t)next_x < width   && 
            0 <= next_y && (size_t)next_y < height  && 
            CELL_AT(next_x, next_y) !=  ASCII_RENDER_SNAKE   ); 
}

snake_state_t move_snake_by(int dx, int dy)
{  
    // save direction
    dir_x        = dx ;
    dir_y        = dy ;
    if (is_safe_move(snake.head->x + dx, snake.head->y + dy)) {   
        if (CELL_AT(snake.head->x + dx, snake.head->y +dy) == ASCII_RENDER_APPLE) {
            // update score
            score++; 
            apple_count = 0;
            // update snake's head
            push_front(&snake, snake.head->x + dx, snake.head->y + dy);
            CELL_AT(snake.head->x, snake.head->y) = ASCII_RENDER_SNAKE;
        } else {
            // update snake's head
            push_front(&snake, snake.head->x + dx, snake.head->y + dy);
            CELL_AT(snake.head->x, snake.head->y) = ASCII_RENDER_SNAKE;
            // update snake's tail
            CELL_AT(snake.tail->x, snake.tail->y) = ASCII_RENDER_EMPTY;
            pop_back(&snake);
        }
        if (snake.size == width * height) 
            return SNAKE_FULL;
        return SNAKE_LIVE;
    } 
    return SNAKE_DEAD;
}

snake_state_t move_right(void)
{
    return move_snake_by(+1, 0);
}

snake_state_t move_up(void)
{
    return move_snake_by(0, -1);
}

snake_state_t move_left(void)
{
    return move_snake_by(-1, 0);
}

snake_state_t move_down(void)
{
    return move_snake_by(0, +1);
}

/*          Queue functions               */

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

/*        Terminal input functions         */
struct termios  saved_attributes = {0};

void init_terminal(void)
{
    // save initial terminal attributes 
    int res = tcgetattr(STDIN_FILENO, &saved_attributes);
    if (res == -1) { 
       perror("tcgetattr : ");
       exit(EXIT_FAILURE);
    }

    struct termios new_attributes = saved_attributes;
    // - do not echo input characters and
    // - disable canonical mode
    // "In noncanonical mode input is available immediately  (without  the  user
    //   having  to type a line-delimiter character)" man page termios, release 4.15
    new_attributes.c_lflag &= ~(ECHO | ICANON);
    
    // apply the new attributes 
    res = tcsetattr(STDIN_FILENO, TCSANOW, &new_attributes); 
    if (res == -1) {
       perror("tcsetattr : ");
       exit(EXIT_FAILURE);
    }
}

void restore_terminal(void) 
{
    // restore initial terminal attributes 
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);   
}

/* wait for maximum @timeout for a key to be pressed
 * return true when a key is pressed and
 *        false otherwise
 */
bool is_key_pressed(void)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    struct timeval timeout = {.tv_sec = 0, .tv_usec = 500000};
    return (select(STDIN_FILENO+1, &readfds, NULL, NULL, &timeout) == 1);
}
