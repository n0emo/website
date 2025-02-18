#include "gamelist.h"

#include "game.h"

// TODO: better design
void comp_game_list(Html *html) {
    html_push_attribute_cstrs(html, "id", "games");
    html_section_begin(html);

    html_h1_begin(html);
    html_text_cstr(html, "Games");
    html_h1_end(html);

    const static CompGame games[] = {
        (CompGame) {
            .name = "Excuse me, what?",
            .desctiption = "Chaotic top-down shooter game",
            .source_code = "https://github.com/n0emo/chaos",
            .itch_io = "https://n0emo.itch.io/excuse-me-what",
            .live_demo = NULL,
        }
    };

    html_push_class_cstr(html, "game-list");
    html_div_begin(html);
    for (size_t i = 0; i < sizeof(games) / sizeof(*games); i++) {
        comp_game(html, games[i]);
    }
    html_div_end(html);

    html_section_end(html);
}
