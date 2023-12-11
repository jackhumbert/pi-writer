#pragma once

#include <stddef.h>
#include <lvgl.h>
#include <tiny_ttf.h>

#define ATKINSON_HYPERLEGIBLE_BOLD_102 ATKINSON_HYPERLEGIBLE_BOLD_102
#define ATKINSON_HYPERLEGIBLE_BOLDITALIC_102 ATKINSON_HYPERLEGIBLE_BOLDITALIC_102
#define ATKINSON_HYPERLEGIBLE_ITALIC_102 ATKINSON_HYPERLEGIBLE_ITALIC_102
#define ATKINSON_HYPERLEGIBLE_REGULAR_102 ATKINSON_HYPERLEGIBLE_REGULAR_102

#define FONT(name, size) \
    extern const unsigned char name##_TTF[]; \
    extern const size_t name##_TTF_SIZE; \
    static lv_font_t * name##_##size; \
    if (! name##_##size ) \
        name##_##size = tiny_ttf_create_data(name##_TTF, name##_TTF_SIZE, size * 1.26)
