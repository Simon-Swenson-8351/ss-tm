#include "ss_tm.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

uint64_t simulated_start_state;

char tape_char_to_symbol(uint64_t tape_char) {
    switch(tape_char) {
        case 0:
            return ' ';
        case 1:
            return '1';
        // Special beginning-of-tape symbol. Needed to simulate the doubly-
        // infinite tape.
        case 2:
            return '[';
        case 3:
            return ']';
        // When we have to shift tape contents over, write a symbol indicating 
        // the state that we left off at.
        case 4:
            return 'a';
        case 5:
            return 'b';
        case 6:
            return 'c';
        default:
            fprintf(stderr, "Unknown tape char %lu\n", tape_char);
            exit(-1);
    }

}

uint64_t symbol_to_tape_char(char c) {
    switch(c) {
        case ' ':
            return 0ull;
        case '1':
            return 1ull;
        case '[':
            return 2ull;
        case ']':
            return 3ull;
        case 'a':
            return 4ull;
        case 'b':
            return 5ull;
        case 'c':
            return 6ull;
        default:
            fprintf(stderr, "Unknown symbol %c\n", c);
            exit(-1);
    }
}

char *state_int_to_str(uint64_t state) {
    if(state == simulated_start_state)
        return "q_0";
    else if(state == simulated_start_state + 1)
        return "q_1";
    else if(state == simulated_start_state + 2)
        return "q_2";
    else if(state == SS_TM_REJECT_STATE)
        return "q_H";
    else {
        fprintf(stderr, "state_int_to_char is only meant to be called with one "
            "of the original four states of the machine.\n");
        exit(-1);
    }
}

char *boolean_to_left_right(bool b) {
    if(b)
        return "right";
    else
        return "left";
}

char *tape_contents_to_string(uint64_t *tape, size_t tape_size) {
    char *result = malloc((tape_size + 1) * sizeof(char));
    size_t i;
    for(i = 0; i < tape_size; i++) {
        result[i] = tape_char_to_symbol(tape[i]);
    }
    result[i] = '\0';
    return result;
}

void add_needed_transitions_for_simulation(struct ss_tm *tm) {
    struct ss_tm_transition trans;
    // States necessary to simulate the SOON-TM:
    // s_0: start state. Shift tape head by one and go to the simulated start 
    //  state. Needed because we start all strings with "beginning of tape" 
    //  symbol.
    // s_1: If we reached beginning of tape symbol.
    // s_2: If we're shifting all characters to the left, and the last character 
    // was a 

    // Since we construct our input to begin with the special beginning-of-tape 
    // symbol, we shift over one space before we begin simulating the SOON-TM.
    trans.in_state = SS_TM_INITIAL_STATE;
    trans.in_char = symbol_to_tape_char('[');
    trans.out_state = simulated_start_state;
    trans.out_char = symbol_to_tape_char('[');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    // Shift all input to the right by one, then return to the beginning, if we 
    // are at the beginning-of-tape character. Need a state to catch this for 
    // each of the normal states, except the halt state.
    uint64_t shift_right_states_start = SS_TM_INITIAL_STATE + 100;

    // Make sure to save our original state (one of the three simulated states, 
    // a, b, or c.
    trans.in_state = simulated_start_state;
    trans.in_char = symbol_to_tape_char('[');
    trans.out_state = shift_right_states_start + symbol_to_tape_char('[');
    trans.out_char = symbol_to_tape_char('a');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = simulated_start_state + 1;
    trans.in_char = symbol_to_tape_char('[');
    trans.out_state = shift_right_states_start + symbol_to_tape_char('[');
    trans.out_char = symbol_to_tape_char('b');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = simulated_start_state + 2;
    trans.in_char = symbol_to_tape_char('[');
    trans.out_state = shift_right_states_start + symbol_to_tape_char('[');
    trans.out_char = symbol_to_tape_char('c');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    // Moving right and we've saved a "["
    trans.in_state = shift_right_states_start + symbol_to_tape_char('[');
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = shift_right_states_start + symbol_to_tape_char(' ');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + symbol_to_tape_char('[');
    trans.in_char = symbol_to_tape_char('1');
    trans.out_state = shift_right_states_start + symbol_to_tape_char('1');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + symbol_to_tape_char('[');
    trans.in_char = symbol_to_tape_char(']');
    trans.out_state = shift_right_states_start + symbol_to_tape_char(']');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    // Moving right and we've saved a " "
    trans.in_state = shift_right_states_start + symbol_to_tape_char(' ');
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = shift_right_states_start + symbol_to_tape_char(' ');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + symbol_to_tape_char(' ');
    trans.in_char = symbol_to_tape_char('1');
    trans.out_state = shift_right_states_start + symbol_to_tape_char('1');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + symbol_to_tape_char(' ');
    trans.in_char = symbol_to_tape_char(']');
    trans.out_state = shift_right_states_start + symbol_to_tape_char(']');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    // Moving right and we've saved a "1"
    trans.in_state = shift_right_states_start + symbol_to_tape_char('1');
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = shift_right_states_start + symbol_to_tape_char(' ');
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + symbol_to_tape_char('1');
    trans.in_char = symbol_to_tape_char('1');
    trans.out_state = shift_right_states_start + symbol_to_tape_char('1');
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + symbol_to_tape_char('1');
    trans.in_char = symbol_to_tape_char(']');
    trans.out_state = shift_right_states_start + symbol_to_tape_char(']');
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    // Moving right and we've saved a "]"
    // Stop moving right. Now we need to return to the saved state.
    trans.in_state = shift_right_states_start + symbol_to_tape_char(']');
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = shift_right_states_start + 50;
    trans.out_char = symbol_to_tape_char(']');
    trans.out_right = false;
    ss_tm_add_state_transition(tm, trans);

    // We've shifted everything to the right one character, now we are returning 
    // to the saved state.
    trans.in_state = shift_right_states_start + 50;
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = shift_right_states_start + 50;
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = false;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + 50;
    trans.in_char = symbol_to_tape_char('1');
    trans.out_state = shift_right_states_start + 50;
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = false;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + 50;
    trans.in_char = symbol_to_tape_char('a');
    trans.out_state = simulated_start_state;
    trans.out_char = symbol_to_tape_char('[');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + 50;
    trans.in_char = symbol_to_tape_char('b');
    trans.out_state = simulated_start_state + 1;
    trans.out_char = symbol_to_tape_char('[');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = shift_right_states_start + 50;
    trans.in_char = symbol_to_tape_char('c');
    trans.out_state = simulated_start_state + 2;
    trans.out_char = symbol_to_tape_char('[');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    // Now, similar if we need to shift the right bound character "]" over by 
    // one.
    uint64_t new_char_on_left_start_state = SS_TM_INITIAL_STATE + 200;

    trans.in_state = simulated_start_state;
    trans.in_char = symbol_to_tape_char(']');
    trans.out_state = new_char_on_left_start_state + symbol_to_tape_char('a');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = simulated_start_state + 1;
    trans.in_char = symbol_to_tape_char(']');
    trans.out_state = new_char_on_left_start_state + symbol_to_tape_char('b');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = simulated_start_state + 2;
    trans.in_char = symbol_to_tape_char(']');
    trans.out_state = new_char_on_left_start_state + symbol_to_tape_char('c');
    trans.out_char = symbol_to_tape_char(' ');
    trans.out_right = true;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = new_char_on_left_start_state + symbol_to_tape_char('a');
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = simulated_start_state;
    trans.out_char = symbol_to_tape_char(']');
    trans.out_right = false;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = new_char_on_left_start_state + symbol_to_tape_char('b');
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = simulated_start_state + 1;
    trans.out_char = symbol_to_tape_char(']');
    trans.out_right = false;
    ss_tm_add_state_transition(tm, trans);

    trans.in_state = new_char_on_left_start_state + symbol_to_tape_char('c');
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = simulated_start_state + 2;
    trans.out_char = symbol_to_tape_char(']');
    trans.out_right = false;
    ss_tm_add_state_transition(tm, trans);
}

void print_state(struct ss_tm *tm, FILE *f) {
    uint64_t *tape;
    size_t tape_size;
    uint64_t state;
    uint64_t head_loc;
    ss_tm_peek_tape_all(tm, &tape, &tape_size);
    ss_tm_peek_state(tm, &state);
    ss_tm_peek_head_pos(tm, &head_loc);

    char *tape_str = tape_contents_to_string(tape, tape_size);
    fprintf(f, "%s\n", tape_str);
    free(tape_str);

    size_t i;
    for(i = 0; i < head_loc; i++) {
        fprintf(f, " ");
    }
    fprintf(f, "^\n");

    for(i = 0; i < head_loc; i++) {
        fprintf(f, " ");
    }
    fprintf(f, "%lu\n", state);
}

void print_simulation_progress(struct ss_tm *tm, uint64_t num_steps, FILE *f) {
    uint64_t i;
    for(i = 0; i < num_steps; i++) {
        uint64_t state;
        if(ss_tm_peek_state(tm, &state) == SS_TM_REJECT_STATE)
            return;
        ss_tm_simulation_step(tm);
        ss_tm_peek_state(tm, &state);
        if(state == simulated_start_state ||
            state == simulated_start_state + 1 ||
            state == simulated_start_state + 2 ||
            state == SS_TM_REJECT_STATE) {
            print_state(tm, f);
        } else {
            i--;
        }
    }
}

// This function assumes that it's in a simulated state (q_0, q_1, q_2, q_R)
void verify_state(struct ss_tm *tm, bool *contains_a_6, bool *contains_no_7s) {

        uint64_t *tape;
        size_t tape_size;
        ss_tm_peek_tape_all(tm, &tape, &tape_size);
        char *tape_str = tape_contents_to_string(tape, tape_size);
        *contains_a_6 = strstr(tape_str, "111111") != NULL;
        *contains_no_7s = strstr(tape_str, "1111111") == NULL;
        free(tape_str);
}

bool verify_simulation_progress(struct ss_tm *tm, uint64_t num_steps) {
    bool there_exists_a_6 = false;
    bool forall_states_no_7s = true;
    uint64_t i;
    for(i = 0; i < num_steps; i++) {
        uint64_t state;
        if(ss_tm_peek_state(tm, &state) == SS_TM_REJECT_STATE)
            return there_exists_a_6 && forall_states_no_7s;
        ss_tm_simulation_step(tm);
        ss_tm_peek_state(tm, &state);
        if(state == simulated_start_state ||
            state == simulated_start_state + 1 ||
            state == simulated_start_state + 2 ||
            state == SS_TM_REJECT_STATE) {

            bool contains_a_6;
            bool contains_no_7s;
            verify_state(tm, &contains_a_6, &contains_no_7s);
            there_exists_a_6 = there_exists_a_6 || contains_a_6;
            forall_states_no_7s = forall_states_no_7s && contains_no_7s;
        } else {
            i--;
        }
    }
    return there_exists_a_6 && forall_states_no_7s;
}

void run_finder() {

    uint64_t states[4];
    states[0] = simulated_start_state;
    states[1] = simulated_start_state + 1;
    states[2] = simulated_start_state + 2;
    states[3] = SS_TM_REJECT_STATE;

    char chars[2] = {' ', '1'};

    bool output_dirs[2] = {false, true};

    // Icky brute force

    int a_blank_output_char;
    int a_blank_output_state;
    int a_blank_output_dir;
    int a_1_output_char;
    int a_1_output_state;
    int a_1_output_dir;
    int b_blank_output_char;
    int b_blank_output_state;
    int b_blank_output_dir;
    int b_1_output_char;
    int b_1_output_state;
    int b_1_output_dir;
    int c_blank_output_char;
    int c_blank_output_state;
    int c_blank_output_dir;
    int c_1_output_char;
    int c_1_output_state;
    int c_1_output_dir;

    uint64_t int_progress = 0;
    double frac_progress = 0.0;
    double prev_frac_progress = 0.0;
    double num_configs = pow(4.0 * 2.0 * 2.0, 6.0);

    for(c_blank_output_char = 0; c_blank_output_char < 2; c_blank_output_char++) {

    for(c_blank_output_state = 0; c_blank_output_state < 4; c_blank_output_state++) {

    for(c_blank_output_dir = 0; c_blank_output_dir < 2; c_blank_output_dir++) {

    for(c_1_output_char = 0; c_1_output_char < 2; c_1_output_char++) {

    for(c_1_output_state = 0; c_1_output_state < 4; c_1_output_state++) {

    for(c_1_output_dir = 0; c_1_output_dir < 2; c_1_output_dir++) {

    for(b_blank_output_char = 0; b_blank_output_char < 2; b_blank_output_char++) {

    for(b_blank_output_state = 0; b_blank_output_state < 4; b_blank_output_state++) {

    for(b_blank_output_dir = 0; b_blank_output_dir < 2; b_blank_output_dir++) {

    for(b_1_output_char = 0; b_1_output_char < 2; b_1_output_char++) {

    for(b_1_output_state = 0; b_1_output_state < 4; b_1_output_state++) {

    for(b_1_output_dir = 0; b_1_output_dir < 2; b_1_output_dir++) {

    for(a_blank_output_char = 0; a_blank_output_char < 2; a_blank_output_char++) {

    for(a_blank_output_state = 0; a_blank_output_state < 4; a_blank_output_state++) {

    for(a_blank_output_dir = 0; a_blank_output_dir < 2; a_blank_output_dir++) {

    for(a_1_output_char = 0; a_1_output_char < 2; a_1_output_char++) {

    for(a_1_output_state = 0; a_1_output_state < 4; a_1_output_state++) {

    for(a_1_output_dir = 0; a_1_output_dir < 2; a_1_output_dir++) {

        prev_frac_progress = frac_progress;
        frac_progress = (int_progress / num_configs) * 100.0;
        if((int)prev_frac_progress != (int)frac_progress) {
            printf("Progress: %2.2f%%\n", frac_progress);
        }
    
        struct ss_tm tm;
        ss_tm_init_begin(&tm);
        add_needed_transitions_for_simulation(&tm);

        struct ss_tm_transition trans;

        trans.in_state = simulated_start_state;
        trans.in_char = symbol_to_tape_char(' ');
        trans.out_state = states[a_blank_output_state];
        trans.out_char = symbol_to_tape_char(chars[a_blank_output_char]);
        trans.out_right = output_dirs[a_blank_output_dir];
        ss_tm_add_state_transition(&tm, trans);

        trans.in_state = simulated_start_state;
        trans.in_char = symbol_to_tape_char('1');
        trans.out_state = states[a_1_output_state];
        trans.out_char = symbol_to_tape_char(chars[a_1_output_char]);
        trans.out_right = output_dirs[a_1_output_dir];
        ss_tm_add_state_transition(&tm, trans);

        trans.in_state = simulated_start_state + 1;
        trans.in_char = symbol_to_tape_char(' ');
        trans.out_state = states[b_blank_output_state];
        trans.out_char = symbol_to_tape_char(chars[b_blank_output_char]);
        trans.out_right = output_dirs[b_blank_output_dir];
        ss_tm_add_state_transition(&tm, trans);

        trans.in_state = simulated_start_state + 1;
        trans.in_char = symbol_to_tape_char('1');
        trans.out_state = states[b_1_output_state];
        trans.out_char = symbol_to_tape_char(chars[b_1_output_char]);
        trans.out_right = output_dirs[b_1_output_dir];
        ss_tm_add_state_transition(&tm, trans);

        trans.in_state = simulated_start_state + 2;
        trans.in_char = symbol_to_tape_char(' ');
        trans.out_state = states[c_blank_output_state];
        trans.out_char = symbol_to_tape_char(chars[c_blank_output_char]);
        trans.out_right = output_dirs[c_blank_output_dir];
        ss_tm_add_state_transition(&tm, trans);

        trans.in_state = simulated_start_state + 2;
        trans.in_char = symbol_to_tape_char('1');
        trans.out_state = states[c_1_output_state];
        trans.out_char = symbol_to_tape_char(chars[c_1_output_char]);
        trans.out_right = output_dirs[c_1_output_dir];
        ss_tm_add_state_transition(&tm, trans);

        ss_tm_init_end(&tm);

        uint64_t input_string[2];
        input_string[0] = symbol_to_tape_char('[');
        input_string[1] = symbol_to_tape_char(']');
        ss_tm_simulation_begin(&tm, input_string, 2);
        bool good = verify_simulation_progress(&tm, 512);
        if(good) {
            char filename[512];
            snprintf(
                filename,
                512,
                "(q_0, ' ')->(%s, %c, %s)_(q_0, '1')->(%s, %c, %s)_"
                "(q_1, ' ')->(%s, %c, %s)_(q_1, '1')->(%s, %c, %s)_"
                "(q_2, ' ')->(%s, %c, %s)_(q_2, '1')->(%s, %c, %s).txt",
                state_int_to_str(states[a_blank_output_state]),
                chars[a_blank_output_char],
                boolean_to_left_right(output_dirs[a_blank_output_dir]),
                state_int_to_str(states[a_1_output_state]),
                chars[a_1_output_char],
                boolean_to_left_right(output_dirs[a_1_output_dir]),
                state_int_to_str(states[b_blank_output_state]),
                chars[b_blank_output_char],
                boolean_to_left_right(output_dirs[b_blank_output_dir]),
                state_int_to_str(states[b_1_output_state]),
                chars[b_1_output_char],
                boolean_to_left_right(output_dirs[b_1_output_dir]),
                state_int_to_str(states[c_blank_output_state]),
                chars[c_blank_output_char],
                boolean_to_left_right(output_dirs[c_blank_output_dir]),
                state_int_to_str(states[c_1_output_state]),
                chars[c_1_output_char],
                boolean_to_left_right(output_dirs[c_1_output_dir])
            );
            FILE *to_write = fopen(filename, "w");
            
            fprintf(to_write, "Possible match.\n");

            uint64_t *tape;
            size_t tape_size;
            ss_tm_peek_tape_all(&tm, &tape, &tape_size);
            char *tape_str = tape_contents_to_string(tape, tape_size);
            fprintf(to_write, "%s\n", tape_str);
            free(tape_str);

            ss_tm_simulation_begin(&tm, input_string, 2);
            print_simulation_progress(&tm, 512, to_write);

            fprintf(to_write, "delta(q_0, ' ') -> (%s, '%c', %s)\n",
                state_int_to_str(states[a_blank_output_state]),
                chars[a_blank_output_char],
                boolean_to_left_right(output_dirs[a_blank_output_dir]));
            fprintf(to_write, "delta(q_0, '1') -> (%s, '%c', %s)\n",
                state_int_to_str(states[a_1_output_state]),
                chars[a_1_output_char],
                boolean_to_left_right(output_dirs[a_1_output_dir]));
            fprintf(to_write, "delta(q_1, ' ') -> (%s, '%c', %s)\n",
                state_int_to_str(states[b_blank_output_state]),
                chars[b_blank_output_char],
                boolean_to_left_right(output_dirs[b_blank_output_dir]));
            fprintf(to_write, "delta(q_1, '1') -> (%s, '%c', %s)\n",
                state_int_to_str(states[b_1_output_state]),
                chars[b_1_output_char],
                boolean_to_left_right(output_dirs[b_1_output_dir]));
            fprintf(to_write, "delta(q_2, ' ') -> (%s, '%c', %s)\n",
                state_int_to_str(states[c_blank_output_state]),
                chars[c_blank_output_char],
                boolean_to_left_right(output_dirs[c_blank_output_dir]));
            fprintf(to_write, "delta(q_2, '1') -> (%s, '%c', %s)\n",
                state_int_to_str(states[c_1_output_state]),
                chars[c_1_output_char],
                boolean_to_left_right(output_dirs[c_1_output_dir]));

            fclose(to_write);
        }

        int_progress++;

        ss_tm_destroy(&tm);
    }

    }

    }

    }

    }

    }

    }

    }

    }

    }

    }

    }

    }

    }

    }

    }

    }

    }
}

void run_verifier() {
    
    struct ss_tm tm;
    ss_tm_init_begin(&tm);
    add_needed_transitions_for_simulation(&tm);

    struct ss_tm_transition trans;

    trans.in_state = simulated_start_state;
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = simulated_start_state + 1;
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = false;
    ss_tm_add_state_transition(&tm, trans);

    trans.in_state = simulated_start_state;
    trans.in_char = symbol_to_tape_char('1');
    trans.out_state = simulated_start_state + 2;
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = false;
    ss_tm_add_state_transition(&tm, trans);

    trans.in_state = simulated_start_state + 1;
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = simulated_start_state + 2;
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = true;
    ss_tm_add_state_transition(&tm, trans);

    trans.in_state = simulated_start_state + 1;
    trans.in_char = symbol_to_tape_char('1');
    trans.out_state = simulated_start_state;
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = false;
    ss_tm_add_state_transition(&tm, trans);

    trans.in_state = simulated_start_state + 2;
    trans.in_char = symbol_to_tape_char(' ');
    trans.out_state = simulated_start_state;
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = false;
    ss_tm_add_state_transition(&tm, trans);

    trans.in_state = simulated_start_state + 2;
    trans.in_char = symbol_to_tape_char('1');
    trans.out_state = simulated_start_state;
    trans.out_char = symbol_to_tape_char('1');
    trans.out_right = true;
    ss_tm_add_state_transition(&tm, trans);

    ss_tm_init_end(&tm);

    uint64_t input_string[2];
    input_string[0] = symbol_to_tape_char('[');
    input_string[1] = symbol_to_tape_char(']');
    ss_tm_simulation_begin(&tm, input_string, 2);
    print_simulation_progress(&tm, 512, stdout);
    ss_tm_destroy(&tm);
}

int main(int argc, char *argv[]) {
    simulated_start_state = SS_TM_INITIAL_STATE + 1000;
    //run_finder();
    run_verifier();
}