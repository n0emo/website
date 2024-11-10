#ifndef WEB_H_
#define WEB_H_

#include "utils.h"
#include "http.h"

bool handle_request(Request *request, Response *response);
void render_index(ArenaStringBuilder *sb);

#endif // WEB_H_

