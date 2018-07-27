#pragma once

#include "exception.h"
#include "keys.h"

#include "ui.h"
#include "os.h"

#include <stdbool.h>
#include <stdint.h>

#if CX_APILEVEL < 8
#error "May only compile with API level 8 or higher; requires newer firmware"
#endif

#define OFFSET_CLA 0
#define OFFSET_INS 1
#define OFFSET_P1 2
#define OFFSET_CURVE 3
#define OFFSET_LC 4
#define OFFSET_CDATA 5

#define INS_VERSION 0x00
#define INS_MAX 0x10
#define INS_EXIT 0x0F

#define ASYNC_PROMPT(screen, ok, cxl) \
    UI_PROMPT(screen, ok, cxl); \
    THROW(ASYNC_EXCEPTION)

// Return number of bytes to transmit (tx)
typedef uint32_t (*apdu_handler)(uint8_t instruction);

void main_loop(apdu_handler handlers[INS_MAX]);

static inline void return_ok(void) {
    THROW(EXC_NO_ERROR);
}

// Send back response; do not restart the event loop
static inline void delayed_send(uint32_t tx) {
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);
}

static inline bool delay_reject(void) {
    G_io_apdu_buffer[0] = EXC_REJECT >> 8;
    G_io_apdu_buffer[1] = EXC_REJECT & 0xFF;
    delayed_send(2);
    return true;
}

uint32_t handle_apdu_error(uint8_t instruction);
uint32_t handle_apdu_version(uint8_t instruction);
uint32_t handle_apdu_exit(uint8_t instruction);

uint32_t send_word_big_endian(uint32_t word);
