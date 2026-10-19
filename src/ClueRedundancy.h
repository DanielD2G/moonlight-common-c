// Clue: redundancy policy for controller state sent unreliably.
// Pure functions over a per-controller state so they can be unit tested on their own.
#pragma once

#include <stdbool.h>
#include <stdint.h>

#define CLUE_RESEND_INTERVAL_US 4000      // re-send the last state this often while active
#define CLUE_ACTIVE_WINDOW_US   500000    // ...for this long after the last state change
#define CLUE_MIN_WAKE_US        1000      // never schedule a resend check closer than this to now

typedef struct _CLUE_PAD_STATE {
    bool valid;            // a state has been seen
    uint32_t lastButtons;
    uint64_t lastChangeUs; // last time a new state arrived
    uint64_t lastSendUs;   // last time this controller's state left for the host
} CLUE_PAD_STATE, *PCLUE_PAD_STATE;

// A new state arrives; returns how many copies to send now (2 when buttons changed, else 1).
int ClueRedundancyOnState(PCLUE_PAD_STATE state, uint32_t buttons, uint64_t nowUs);

// The controller's state was sent at nowUs.
void ClueRedundancyOnSent(PCLUE_PAD_STATE state, uint64_t nowUs);

// True when the last state should be sent again: input was active recently and the resend
// interval has passed since the last send.
bool ClueRedundancyShouldResend(PCLUE_PAD_STATE state, uint64_t nowUs);

// Absolute time at which the input send thread should next look for resends, or 0 when no
// controller is active (the thread then blocks on its queue with no timeout, as upstream does).
// Never earlier than nowUs + CLUE_MIN_WAKE_US, so a controller whose new state is still queued
// (not sent yet) cannot make the thread spin on a deadline in the past.
uint64_t ClueRedundancyNextCheckUs(const CLUE_PAD_STATE* pads, int count, uint64_t nowUs);
