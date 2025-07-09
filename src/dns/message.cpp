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
    Message::~Message() {
        for (auto &data : this->answers_data)
        {
            delete[] data;
            data = nullptr;
        }
    }

    void Message::resolve(){
        this->header.query_response = 1;
        this->header.answer_count = this->header.question_count;
        for (int i = 0; i < this->header.question_count; i++)
        {
            Answer answer;
            answer.domain_name = this->questions[i].domain_name;
            answer.domain_type = this->questions[i].domain_type;
            answer.domain_class = this->questions[i].domain_class;
            answer.ttl = 3600;
            answer.data_length = 4;
            answer.data = new uint8_t[4];
            this->answers_data.emplace_back(answer.data);
            answer.data[0] = 127;
            answer.data[1] = 0;
            answer.data[2] = 0;
            answer.data[3] = 1;
            this->answers.emplace_back(answer);
        }
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
    std::string Message::to_string()
    {
        std::string result = "";
        if (this->questions.size())
        {
            result += "Questions:\n";
            for (auto &question : this->questions)
            {
                result += "\t" + question.to_string() + "\n";
            }
        }
        if (this->answers.size())
        {
            result += "Answers:\n";
            for (auto &answer : this->answers)
            {
                result += "\t" + answer.to_string() + "\n";
            }
        }
        return result;
    }
}
