#include "ClueRedundancy.h"

int ClueRedundancyOnState(PCLUE_PAD_STATE state, uint32_t buttons, uint64_t nowUs) {
    int copies = (state->valid && state->lastButtons != buttons) ? 2 : 1;
    state->valid = true;
    state->lastButtons = buttons;
    state->lastChangeUs = nowUs;
    return copies;
}

void ClueRedundancyOnSent(PCLUE_PAD_STATE state, uint64_t nowUs) {
    state->lastSendUs = nowUs;
}

bool ClueRedundancyShouldResend(PCLUE_PAD_STATE state, uint64_t nowUs) {
    return state->valid &&
           nowUs - state->lastChangeUs < CLUE_ACTIVE_WINDOW_US &&
           nowUs - state->lastSendUs >= CLUE_RESEND_INTERVAL_US;
}

uint64_t ClueRedundancyNextCheckUs(const CLUE_PAD_STATE* pads, int count, uint64_t nowUs) {
    uint64_t next = 0;
    for (int i = 0; i < count; i++) {
        if (!pads[i].valid || nowUs - pads[i].lastChangeUs >= CLUE_ACTIVE_WINDOW_US) {
            continue;
        }
        uint64_t due = pads[i].lastSendUs + CLUE_RESEND_INTERVAL_US;
        if (due < nowUs + CLUE_MIN_WAKE_US) {
            due = nowUs + CLUE_MIN_WAKE_US;
        }
        if (next == 0 || due < next) {
            next = due;
        }
    }
    return next;
}
