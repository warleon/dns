#pragma once

#include "imports.hpp"
#include "header.hpp"
#include "question.hpp"
#include "answer.hpp"
namespace DNS
{

struct Message
{
    Header header = {};
    std::vector<Question> questions = {};
    std::vector<Answer> answers = {};
    std::vector<uint8_t*> answers_data = {};
    
    Message();
    Message(const buffer_t buffer);

    ~Message();

    uint16_t from_buffer(const buffer_t buffer);
    void resolve();
    uint16_t to_buffer(buffer_t buffer);
    std::string to_string();
};
}