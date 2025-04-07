#include "queue.h"
#include "tile_game.h"

void enqueue(struct queue *q, struct game_state state) {
    uint64_t serialized_state = serialize(state);
    insert_at_head(&q->data, serialized_state);
}

struct game_state dequeue(struct queue *q) {
    if (q->data.head == NULL) {
        return(struct game_state){0};
    }
    uint64_t serialized_state = remove_from_tail(&q->data);
    return deserialize(serialized_state);
}

bool gs_solved(struct game_state* state) {
    static const uint8_t final_tiles[4][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 0}
    };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (state->tiles[i][j] != final_tiles[i][j]) {
                return false;
            }
        }
    }
    return state->empty_row == 3 && state->empty_col == 3;
}

bool state_in_list(struct queue q, struct game_state curr) {
    struct list_node *cur = q.data.head;
    uint64_t curr_hash = serialize(curr);
    while (cur != NULL) {
        if (cur->value == curr_hash) {
            return true;
        }
        cur = cur->next;
    }
    return false;
}

int number_of_moves(struct game_state start) {
    if (gs_solved(&start)) {
        return 0;
    }
    struct queue q = {0};
    struct queue visited = {0};

    enqueue(&q, start);
    enqueue(&visited, start);
    while (q.data.head != NULL) {
        struct game_state curr = dequeue(&q);
        if (gs_solved(&curr)) {
            free_list(q.data);
            free_list(visited.data);
            return curr.num_steps;
        }
           
        //move up
        struct game_state next = curr;
        move_up(&next);
        // check if the state has been visited
        if (!state_in_list(visited, next)) {
            enqueue(&q, next);
            enqueue(&visited, next);
        }

        //move down
        next = curr;
        move_down(&next);
        // check if the state has been visited
        if (!state_in_list(visited, next)) {
            enqueue(&q, next);
            enqueue(&visited, next);
        }

        //move left
        next = curr;
        move_left(&next);
        // check if the state has been visited
        if (!state_in_list(visited, next)) {
            enqueue(&q, next);
            enqueue(&visited, next);
        }

        //move right
        next = curr;
        move_right(&next);
        // check if the state has been visited
        if (!state_in_list(visited, next)) {
            enqueue(&q, next);
            enqueue(&visited, next);
        }
    }
    free_list(q.data);
    free_list(visited.data);
    return 0;
}
