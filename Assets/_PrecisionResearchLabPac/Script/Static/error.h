#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <string.h>

static char _last_error_message[256] = "";

/* macro */
#define SET_ERROR_MESSAGE(msg) \
snprintf(_last_error_message, sizeof(_last_error_message), \
"[%s:%d %s] %s", __FILE__, __LINE__, __func__, msg)

#define SET_ERROR_MESSAGE_ARGUMENT(msg, ...) \
snprintf(_last_error_message, sizeof(_last_error_message), \
"[%s:%d %s] " msg, __FILE__, __LINE__, __func__, __VA_ARGS__)

// エラーメッセージをリセットする（内部用）
static void clear_error_message(void) {
    _last_error_message[0] = '\0';
}

#endif