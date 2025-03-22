#ifndef MEW_INCLUDE_MEW_INI_H_
#define MEW_INCLUDE_MEW_INI_H_

#include "mew/alloc.h"
#include "mew/str.h"
#include "mew/utils.h"

typedef struct {
    StringView key;
    StringView value;
} IniItem;

typedef struct {
    IniItem *items;
    size_t count;
    size_t capacity;
} IniItems;

typedef struct {
    StringView name;
    IniItems items;
} IniSection;

typedef struct {
    IniSection *items;
    size_t count;
    size_t capacity;
} IniSections;

typedef struct {
    Allocator alloc;
    IniSections sections;
} Ini;

bool parse_ini(StringView text, Ini *ini);
IniSection *ini_get_section(Ini ini, StringView name);
IniItem *ini_get_item(IniSection section, StringView name);

#endif // MEW_INCLUDE_MEW_INI_H_
