#include "mew/http/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *http_status_desc(HttpStatus status) {
    switch (status) {
        case HTTP_OK:                    return "OK";
        case HTTP_NOT_FOUND:             return "NOT FOUND";
        case HTTP_INTERNAL_SERVER_ERROR: return "INTERNAL SERVER ERROR";
    }

    return "UNKNOWN";
}

const char *http_method_str(HttpMethod method) {
    switch (method) {
        case HTTP_GET:  return "GET";
        case HTTP_POST: return "POST";
    }

    return "UNKNOWN";
}

bool http_urldecode(StringView sv, StringBuilder *out) {
    static const char *allowed =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789-._~"
        "!#$&'()*+,/:;=?@[]";

    while (sv.count > 0) {
        char c = sv.items[0];
        if (strchr(allowed, c)) {
            sb_append_char(out, c);
            sv = sv_slice_from(sv, 1);
        } else if (c == '%') {
            if (sv.count < 3) return false;
            char digits[3] = {0};
            digits[0] = sv.items[1];
            digits[1] = sv.items[2];
            char result = (char) strtol(digits, NULL, 16);
            if (result == 0) return false;
            sb_append_char(out, result);
            sv = sv_slice_from(sv, 3);
        } else {
            return false;
        }
    }

    return true;
}

void http_urlencode(StringView sv, StringBuilder *out) {
    static const char *allowed =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789-._~";

    static const char hex[] = "0123456789ABCDEF";

    while (sv.count > 0) {
        char c = sv.items[0];
        if (strchr(allowed, c)) {
            sb_append_char(out, c);
        } else {
            unsigned char u = (unsigned char) c;
            sb_append_char(out, '%');
            sb_append_char(out, hex[(u & 0xF0) >> 4]);
            sb_append_char(out, hex[u & 0x0F]);
        }
        sv = sv_slice_from(sv, 1);
    }
}
