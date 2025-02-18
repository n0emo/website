#ifndef WEBSITE_SRC_COMPONENTS_GAME_H_
#define WEBSITE_SRC_COMPONENTS_GAME_H_

#include "mew/html.h"

typedef struct CompGame {
    const char *name;
    const char *desctiption;
    const char *source_code;
    const char *itch_io;
    const char *live_demo;
} CompGame;

void comp_game(Html *html, CompGame game);

#endif // WEBSITE_SRC_COMPONENTS_GAME_H_
