#include "about.h"

#include "components/technology.h"

void comp_about(Html *html) {
    html_push_class_cstr(html, "about");
    html_push_attribute_cstrs(html, "id", "about");
    html_section_begin(html);

    html_h1_begin(html);
    html_text_cstr(html, "About");
    html_h1_end(html);

    html_push_class_cstr(html, "content");
    html_div_begin(html);

    html_push_class_cstr(html, "profile");
    html_div_begin(html);

    html_push_attribute_cstrs(html, "src", "/logos/Person.svg");
    html_img_short(html);

    html_p_begin(html);
    html_text_cstr(
        html,
        "I'm Albert Shefner. I'm a "
        "software engineer interested in many areas, such as: low-level, "
        "system and audio programming; web and game development. Can work "
        "with any enough documented piece of technology."
    );
    html_p_end(html);

    html_div_end(html);

    static const char *technologies[] = { "WebGPU", "C", "Tokio", "Git", "Python", "C#", "Rust", "C++", "JavaScript", "Clojure", "Go" };
    html_push_class_cstr(html, "technology-grid");
    html_div_begin(html);
    for (size_t i = 0; i < sizeof(technologies) / sizeof(technologies[0]); i++) {
        comp_technology(html, technologies[i]);
    }
    html_div_end(html);

    html_div_end(html);

    html_section_end(html);
}
