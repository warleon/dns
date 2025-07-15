#pragma once

#include "imports.hpp"
#include "lable.hpp"

namespace DNS
{

struct Question
{
    Label::list_t domain_name;
    uint16_t domain_type;
    uint16_t domain_class;

    uint16_t from_buffer(const buffer_t buffer, uint16_t offset );
    uint16_t to_buffer(buffer_t buffer, uint16_t offset );
    std::string to_string();

};
}