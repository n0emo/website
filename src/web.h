#ifndef WEB_H_
#define WEB_H_

#include "html.h"
#include "http.h"
#include "utils.h"

bool handle_request(Request *request, Response *response);
void render_index(ArenaStringBuilder *sb);
void render_blogs(ArenaStringBuilder *sb);
void page_base_begin(Html *html, char *title);
void page_base_end(Html *html);
#endif // WEB_H_

