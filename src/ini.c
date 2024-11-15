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
            ARRAY_APPEND_ARENA(&ini->sections, section, ini->arena);
            current = &ini->sections.items[ini->sections.count - 1];
        } else if (current == NULL) {
            return false;
        } else {
            StringView key = sv_trim_space(sv_chop_by(&line, '='));
            StringView value = sv_trim_space(line);

            if (value.count == 0) return false;

            IniItem item = { .key = key, .value = value };
            ARRAY_APPEND_ARENA(&current->items, item, ini->arena);
        }
    }

    return true;
}
