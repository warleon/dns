#pragma once

#include "imports.hpp"
#include "header.hpp"
#include "question.hpp"

namespace DNS
{

struct Message
{
    Header header = {};
    std::vector<Question> questions = {};

    Message();
    Message(const buffer_t buffer);
    Message(Message &query);

    ~Message();

    uint16_t from_buffer(const buffer_t buffer);
    void from_question(Message &query);
    uint16_t to_buffer(buffer_t buffer);

};
}