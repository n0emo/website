#include "contact_info.h"

void comp_contact_info(Html *html) {
    html_push_attribute_cstrs(html, "id", "contact");
    html_section_begin(html);

    html_h1_begin(html);
    html_text_cstr(html, "Contact");
    html_h1_end(html);

    html_p_begin(html);
    html_text_cstr(html, "Email: solaris945[at]gmail.com");
    html_p_end(html);

    html_p_begin(html);
    html_text_cstr(html, "Telegram: @omega_lolxd");
    html_p_end(html);

    html_section_end(html);
}
