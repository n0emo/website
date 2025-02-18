#ifndef WEBSITE_SRC_COMPONENTS_PROJECT_H_
#define WEBSITE_SRC_COMPONENTS_PROJECT_H_

#include "mew/html.h"

typedef struct CompProject {
    const char *name;
    const char *desctiption;
    const char *source_code;
    const char *image_src;
    const char *live_demo_src;
} CompProject;

void comp_project(Html *html, CompProject project);

#endif // WEBSITE_SRC_COMPONENTS_PROJECT_H_
