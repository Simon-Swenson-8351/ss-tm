#ifndef ss_tm_h
#define ss_tm_h

#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

const uint64_t SS_TM_INITIAL_STATE = 0ull;
const uint64_t SS_TM_ACCEPT_STATE = 0xFFFFFFFFFFFFFFFF;
const uint64_t SS_TM_REJECT_STATE = 0xFFFFFFFFFFFFFFFE;

const bool SS_TM_DIR_LEFT = false;
const bool SS_TM_DIR_RIGHT = true;

enum ss_tm_err {
    SS_TM_ERR_NO_ERROR,
    SS_TM_ERR_ALLOCATION_FAILED,
    SS_TM_ERR_ADDING_ALREADY_EXISTING_STATE,
    SS_TM_ERR_MACHINE_ALREADY_INITIALIZED,
    SS_TM_ERR_UNACCEPTABLE_INPUT_CHAR,
    SS_TM_ERR_UNSTARTED_SIMULATION,
    SS_TM_ERR_STEP_ON_HALTED_MACHINE
};

char *ss_tm_err_str[] = {
    "ss_tm: no error",
    "ss_tm: memory allocation failed",
    "ss_tm: The inputs of the given transition matched the inputs of an already"
        "of a transition already in the transition map.",
    "ss_tm: The machine has already been initialized. (You're probably trying "
        "to perform an action that can only be done in initialization.)",
    "ss_tm: The input string contained an invalid character. Recall that "
        "input characters must be in the range 1..(max(uint64_t)/2)",
    "ss_tm: The simulation hasn't been started. (You're probably trying to "
        "perform an action that can only be done after a simulation has been "
        "started.)",
    "ss_tm: The machine has halted, but you attempted to perform a simulation "
        "step."
};

struct ss_tm_transition {
    uint64_t in_state;
    uint64_t in_char;
    uint64_t out_state;
    uint64_t out_char;
    // true: move right, false: move left.
    bool out_right;
};

struct ss_tm {
    // States are assumed to range over 0..max(uint64_t)
    // Initial, accept, and reject state constants (above) should be used for those.
    // The input alphabet is assumed to range over 1..(max(uint64_t) / 2)
    // The tape alphabet is assumed to range over 0..max(uint64_t).
    // The tape's blank symbol is assumed to map to 0.

    bool init;
    struct ss_tm_transition *transitions;
    size_t transitions_end;
    size_t transitions_size;

    // For simulations
    bool simulation_started;
    uint64_t *tape;
    size_t tape_size;
    size_t tape_head;

    uint64_t state;
};

// Any function def found between init_begin and init_end should only be called 
// when the object is being initialized. Any function def found outside that 
// range should only be called after the object's initialization is complete.
    enum ss_tm_err
ss_tm_init_begin(
    struct ss_tm *self);

// In running the simulation, if a matching input combination is not found, the 
// machine will go to the reject state and halt. That is, missing state 
// transitions are assumed to always reject.
    enum ss_tm_err
ss_tm_add_state_transition(
    struct ss_tm *self,
    struct ss_tm_transition to_add);

    enum ss_tm_err
ss_tm_init_end(
    struct ss_tm *self);
// End init definitions

    enum ss_tm_err
ss_tm_destroy(
    struct ss_tm *self);

// Begin simulation definitions
    enum ss_tm_err
ss_tm_simulation_begin(
    struct ss_tm *self,
    uint64_t *input_string,
    size_t input_string_size);

    enum ss_tm_err
ss_tm_simulation_step(
    struct ss_tm *self);

    enum ss_tm_err
ss_tm_simulation_step_multiple(
    struct ss_tm *self,
    uint64_t num_steps);
// End simulation definitions

// Begin configuration peeking definitions
    enum ss_tm_err
ss_tm_peek_tape_char(
    struct ss_tm *self,
    size_t in_position,
    uint64_t *out_char);

    enum ss_tm_err
ss_tm_peek_tape_all(
    struct ss_tm *self,
    uint64_t **out_tape,
    size_t *out_tape_size);

    enum ss_tm_err
ss_tm_peek_state(
    struct ss_tm *self,
    uint64_t *state);

    enum ss_tm_err
ss_tm_peek_head_pos(
    struct ss_tm *self,
    uint64_t *head_pos);
// End configuration peeking definitions

#endif // #ifndef ss_tm_h