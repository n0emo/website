#include "projectlist.h"

#include "components/project.h"

// TODO: better design
void comp_project_list(Html *html) {
    html_push_attribute_cstrs(html, "id", "projects");
    html_section_begin(html);

    html_h1_begin(html);
    html_text_cstr(html, "Projects");
    html_h1_end(html);

    const static CompProject projects[] = {
        (CompProject) {
            .name = "Compressor",
            .desctiption = "Dynamic compressor CLAP plugin in C",
            .source_code = "https://github.com/n0emo/compressor",
            .image_src = NULL,
            .live_demo_src = NULL,
        },
        (CompProject) {
            .name = "Hyperclip",
            .desctiption = "Distortion CLAP/VST plugin",
            .source_code = "https://github.com/n0emo/hyperclip",
            .image_src = NULL,
            .live_demo_src = NULL,
        },
        (CompProject) {
            .name = "NBS",
            .desctiption = "Build system to build C++ using C++",
            .source_code = "https://github.com/n0emo/compressor",
            .image_src = NULL,
            .live_demo_src = NULL,
        },
    };

    html_push_class_cstr(html, "project-list");
    html_div_begin(html);
    for (size_t i = 0; i < sizeof(projects) / sizeof(*projects); i++) {
        comp_project(html, projects[i]);
    }
    html_div_end(html);

    html_section_end(html);
}
