#include "message.hpp"

namespace DNS
{


    Message::Message() {}
    Message::Message(const buffer_t buffer)
    {
        this->from_buffer(buffer);
    }
    uint16_t Message::from_buffer(const buffer_t buffer)
    {
        uint16_t offset = 0;
        offset += this->header.from_buffer(buffer);
        questions.reserve(this->header.question_count);
        for (int i = 0; i < this->header.question_count; i++)
        {
            Question question;
            offset += question.from_buffer(buffer + offset);
            questions.emplace_back(question);
        }
        return offset;
    }
    Message::Message(Message &query)
    {
        this->from_question(query);
    }
    Message::~Message() {}

    void Message::from_question(Message &query)
    {
        memcpy(&this->header, &query.header, sizeof(Header));
        this->header.query_response = 1;
        this->questions = query.questions;
    }
    uint16_t Message::to_buffer(buffer_t buffer)
    {
        uint16_t offset = 0;
        offset += this->header.to_buffer(buffer);
        for (auto &question : this->questions)
        {
            offset += question.to_buffer(buffer + offset);
        }
        return offset;
    }
}
