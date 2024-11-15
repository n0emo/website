#include "ini.h"

// TODO: nesting
bool parse_ini(StringView text, Ini *ini) {
    IniSection *current = NULL;

    while (text.count > 0) {
        StringView line = sv_chop_by(&text, '\n');
        line = sv_chop_by(&line, ';');
        line = sv_trim_space(line);

        if (line.count == 0) {
            continue;
        } else if (line.items[0] == '[') {
            if (line.count < 3) return false;
            if (line.items[line.count - 1] != ']') return false;
            StringView section_name = sv_slice(line, 1, line.count - 2);
            IniSection section = {
                .name = sv_trim_space(section_name),
                .items = {0},
            };
            ARRAY_APPEND(&ini->sections, section, ini->alloc);
            current = &ini->sections.items[ini->sections.count - 1];
        } else if (current == NULL) {
            return false;
        } else {
            StringView key = sv_trim_space(sv_chop_by(&line, '='));
            StringView value = sv_trim_space(line);

            if (value.count == 0) return false;

            IniItem item = { .key = key, .value = value };
            ARRAY_APPEND(&current->items, item, ini->alloc);
        }
    }

    return true;
}


IniSection *ini_get_section(Ini ini, StringView name) {
    for (size_t i = 0; i < ini.sections.count; i++) {
        if (sv_eq_sv(ini.sections.items[i].name, name)) {
            return &ini.sections.items[i];
        }
    }

    return NULL;
}

IniItem *ini_get_item(IniSection section, StringView name) {
    for (size_t i = 0; i < section.items.count; i++) {
        if (sv_eq_sv(section.items.items[i].key, name)) {
            return &section.items.items[i];
        }
    }

    return NULL;
}
