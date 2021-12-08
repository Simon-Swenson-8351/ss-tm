#include "ss_tm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

    enum ss_tm_err
ss_tm_init_begin(
    struct ss_tm *self) {

    self->init = false;
    self->transitions = (struct ss_tm_transition *)malloc(sizeof(struct ss_tm_transition) * 16);
    if(!self->transitions)
        return SS_TM_ERR_ALLOCATION_FAILED;
    self->transitions_end = 0;
    self->transitions_size = 16;

    self->simulation_started = false;
    self->tape = NULL;

    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_add_state_transition(
    struct ss_tm *self,
    struct ss_tm_transition to_add) {

    int i;
    for(i = 0; i < self->transitions_end; i++) {
        if(self->transitions[i].in_state == to_add.in_state &&
            self->transitions[i].in_char == to_add.in_char) {
            
            return SS_TM_ERR_ADDING_ALREADY_EXISTING_STATE;
        }
    }

    if(self->init) {
        return SS_TM_ERR_MACHINE_ALREADY_INITIALIZED;
    }

    if(self->transitions_size == self->transitions_end) {
        self->transitions = (struct ss_tm_transition *)realloc(
            self->transitions,
            sizeof(struct ss_tm_transition) * self->transitions_size * 2);
        if(!self->transitions)
            return SS_TM_ERR_ALLOCATION_FAILED;
        self->transitions_size = self->transitions_size * 2;
    }
    self->transitions[self->transitions_end++] = to_add;
    
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_init_end(
    struct ss_tm *self) {

    if(self->init) {
        return SS_TM_ERR_MACHINE_ALREADY_INITIALIZED;
    }
    self->init = true;
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_simulation_begin(
    struct ss_tm *self,
    uint64_t *input_string,
    size_t input_string_size) {

    // Check if the input string contains only valid characters: 
    // (1..(max(uint64_t) / 2))
    int i;
    for(i = 0; i < input_string_size; i++) {
        if(input_string[i] < 1 || input_string[i] > 0x7FFFFFFFFFFFFFFF)
            return SS_TM_ERR_UNACCEPTABLE_INPUT_CHAR;
    }

    free(self->tape);
    // calloc needed, since tape empty char is assumed to be 0.
    self->tape = calloc(input_string_size, sizeof(uint64_t));
    if(!self->tape)
        return SS_TM_ERR_ALLOCATION_FAILED;
    memcpy(self->tape, input_string, input_string_size * sizeof(uint64_t));
    self->tape_size = input_string_size;
    self->tape_head = 0;
    self->state = SS_TM_INITIAL_STATE;
    self->simulation_started = true;
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_simulation_step(
    struct ss_tm *self) {

    if(!self->simulation_started)
        return SS_TM_ERR_UNSTARTED_SIMULATION;

    if(self->state == SS_TM_ACCEPT_STATE ||
        self->state == SS_TM_REJECT_STATE) {
        
        return SS_TM_ERR_STEP_ON_HALTED_MACHINE;
    }

    int i;
    for(i = 0; i < self->transitions_end; i++) {
        if(self->transitions[i].in_state == self->state &&
            self->transitions[i].in_char == self->tape[self->tape_head]) {
            
            self->state = self->transitions[i].out_state;
            self->tape[self->tape_head] = self->transitions[i].out_char;
            if(self->transitions[i].out_right) {
                self->tape_head++;
                if(self->tape_head == self->tape_size) {
                    self->tape = realloc(self->tape, 2 * self->tape_size * sizeof(uint64_t));
                    memset(self->tape + self->tape_head, 0x00, self->tape_size * sizeof(uint64_t));
                    self->tape_size *= 2;
                    if(!self->tape)
                        return SS_TM_ERR_ALLOCATION_FAILED;
                }
            } else {
                if(self->tape_head == 0) {
                    fprintf(stderr, "Caller's TM is malformed. Attempted to move the "
                        "head to the left when already on the left-most cell.\n");
                    exit(-1);
                }
                self->tape_head--;
            }
            return SS_TM_ERR_NO_ERROR;
        }
    }
    self->state = SS_TM_REJECT_STATE;
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_simulation_step_multiple(
    struct ss_tm *self,
    uint64_t num_steps) {

    uint64_t i;
    for(i = 0; i < num_steps; i++) {
        enum ss_tm_err e = ss_tm_simulation_step(self);
        if(e != SS_TM_ERR_NO_ERROR)
            return e;
    }
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_peek_tape_char(
    struct ss_tm *self,
    size_t in_position,
    uint64_t *out_char) {

    if(!self->simulation_started)
        return SS_TM_ERR_UNSTARTED_SIMULATION;

    if(in_position >= self->tape_size) {
        *out_char = 0ull;
    } else {
        *out_char = self->tape[in_position];
    }
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_peek_tape_all(
    struct ss_tm *self,
    uint64_t **out_tape,
    size_t *out_tape_size) {

    if(!self->simulation_started)
        return SS_TM_ERR_UNSTARTED_SIMULATION;

    *out_tape = self->tape;
    *out_tape_size = self->tape_size;
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_peek_state(
    struct ss_tm *self,
    uint64_t *state) {

    if(!self->simulation_started)
        return SS_TM_ERR_UNSTARTED_SIMULATION;

    *state = self->state;
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_peek_head_pos(
    struct ss_tm *self,
    uint64_t *head_pos) {

    if(!self->simulation_started)
        return SS_TM_ERR_UNSTARTED_SIMULATION;

    *head_pos = self->tape_head;
    return SS_TM_ERR_NO_ERROR;
}

    enum ss_tm_err
ss_tm_destroy(
    struct ss_tm *self) {

    free(self->transitions);
    free(self->tape);
    return SS_TM_ERR_NO_ERROR;
}