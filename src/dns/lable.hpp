#pragma once

#include "imports.hpp"

namespace DNS
{

struct Label
{
    uint8_t length;
    char *name;

    uint16_t from_buffer(const buffer_t buffer);
    uint16_t to_buffer(buffer_t buffer);
};
}