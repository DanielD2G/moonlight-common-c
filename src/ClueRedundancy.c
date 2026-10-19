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
