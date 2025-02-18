#include "footer.h"

void comp_footer(Html *html) {
    html_footer_begin(html);
    html_push_class_cstr(html, "footer-text");
    html_div_begin(html);
    html_p_begin(html);
    html_text_cstr(html, "Ancient technologies are being used to show this page for you.");
    html_p_end(html);
    html_hyperlink_cstr(html, "Source code", "https://github.com/n0emo/website");
    html_div_end(html);
    html_footer_end(html);
}

