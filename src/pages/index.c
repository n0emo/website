#include "index.h"

#include "base.h"

#include "components/about.h"
#include "components/contact_info.h"
#include "components/gamelist.h"
#include "components/projectlist.h"

void page_index(StringBuilder *sb) {
    Html html = html_begin();
    page_base_begin(&html, cstr_to_sv("_n0emo website"));

    html_push_class_cstr(&html, "index");
    html_div_begin(&html);

    comp_about(&html);
    comp_project_list(&html);
    comp_game_list(&html);
    comp_contact_info(&html);

    html_div_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}
