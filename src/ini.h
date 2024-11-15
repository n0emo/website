#ifndef INI_H_
#define INI_H_

#include "utils.h"
#include "alloc.h"

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
    Allocator *alloc;
    IniSections sections;
} Ini;

bool parse_ini(StringView text, Ini *ini);

#endif // INI_H_
