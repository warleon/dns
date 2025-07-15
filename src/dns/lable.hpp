#pragma once

#include "imports.hpp"

namespace DNS
{

struct Label
{
    typedef std::list<Label> list_t;
    
    uint8_t length;
    uint8_t *name;

    uint16_t from_buffer(const buffer_t buffer, uint16_t offset );
    uint16_t to_buffer(buffer_t buffer, uint16_t offset ) const;

    static uint16_t list_from_buffer(const buffer_t buffer, list_t &labels, uint16_t offset );
    static uint16_t list_to_buffer(const list_t &labels, buffer_t buffer, uint16_t offset );
    static std::string list_to_string(const list_t &labels);
};
}