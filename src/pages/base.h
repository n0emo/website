#ifndef WEBSITE_PAGES_BASE_H_
#define WEBSITE_PAGES_BASE_H_

#include "mew/html.h"

void page_base_begin(Html *html, StringView title);
void page_base_end(Html *html);

#endif // WEBSITE_PAGES_BASE_H_
