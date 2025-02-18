#include "music.h"

#include "components/linkitem.h"

void render_music(StringBuilder *sb) {
    Html html = html_begin();
    page_base_begin(&html, cstr_to_sv("Music"));

    html_h1_begin(&html);
    html_text_cstr(&html, "There will be some stuff about my music");
    html_h1_end(&html);

    html_h2_begin(&html);
    html_text_cstr(&html, "Listen:");
    html_h2_end(&html);

    html_ul_begin(&html);
    comp_link_item(&html, "YouTube", "https://youtube.com/channel/UCuYpUcLPU_0xn1jZJrRiyEQ?si=8SOVgU9d5C0KHAzD");
    comp_link_item(&html, "Yandex Music", "https://music.yandex.ru/artist/17650218");
    comp_link_item(&html, "VK", "https://vk.com/artist/_n0emo");
    html_ul_end(&html);

    page_base_end(&html);
    html_render_to_sb_and_free(&html, sb);
}
