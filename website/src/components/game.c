#include "game.h"

void comp_game(Html *html, CompGame game) {
    html_push_class_cstr(html, "game");
    html_div_begin(html);

    html_push_class_cstr(html, "title");
    html_hyperlink_cstr(html, game.name, game.source_code);
    html_p_begin(html);
    html_text_cstr(html, game.desctiption);
    html_p_end(html);

    if (game.itch_io) {
        html_hyperlink_cstr(html, "itch.io", game.itch_io);
    }

    html_div_end(html);
}
