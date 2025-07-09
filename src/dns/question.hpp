#pragma once

#include "imports.hpp"
#include "lable.hpp"
#include "header.hpp"

namespace DNS
{

struct Question
{
    std::list<Label> domain_name;
    uint16_t question_type;
    uint16_t question_class;

    uint16_t from_buffer(const buffer_t buffer);
    uint16_t to_buffer(buffer_t buffer);
    std::string to_string();

};
}