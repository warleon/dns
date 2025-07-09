#pragma once

#include "imports.hpp"

namespace DNS
{

struct Label
{
    typedef std::list<Label> list_t;
    
    uint8_t length;
    char *name;

    uint16_t from_buffer(const buffer_t buffer);
    uint16_t to_buffer(buffer_t buffer);

    static uint16_t list_from_buffer(const buffer_t buffer, list_t &labels);
};
}