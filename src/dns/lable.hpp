#pragma once

#include "imports.hpp"

namespace DNS
{

struct Label
{
    typedef std::list<Label> list_t;
    
    uint8_t length;
    uint8_t *name;

    uint16_t from_buffer(const buffer_t buffer);
    uint16_t to_buffer(buffer_t buffer) const;

    static uint16_t list_from_buffer(const buffer_t buffer, list_t &labels);
    static uint16_t list_to_buffer(const list_t &labels, buffer_t buffer);
    static std::string list_to_string(const list_t &labels);
};
}