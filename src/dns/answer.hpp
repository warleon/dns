#pragma once

#include "imports.hpp"
#include "lable.hpp"

namespace DNS
{

struct Answer
{
    Label::list_t domain_name;
    uint16_t domain_type;
    uint16_t domain_class;
    uint32_t ttl;
    uint16_t data_length;
    uint8_t *data;

    uint16_t from_buffer(const buffer_t buffer);
    uint16_t to_buffer(buffer_t buffer);
    std::string to_string();
};
}