#include "question.hpp"

namespace DNS
{

    uint16_t Question::from_buffer(const buffer_t buffer)
    {
        uint16_t offset = 0;
        for (int i = 0; i < buffer_size - sizeof(Header); i++)
        {
            Label label;
            uint16_t label_size = label.from_buffer(buffer + offset);
            if (label_size == 0)
            {
                offset++;
                break;
            }
            offset += label_size;
            this->domain_name.push_back(label);
        }
        const uint16_t* buffer_ptr = reinterpret_cast<const uint16_t*>(buffer + offset);
        this->question_type = ntohs(buffer_ptr[0]);
        this->question_class = ntohs(buffer_ptr[1]);
        return offset + 4;
    }

    uint16_t Question::to_buffer(buffer_t buffer)
    {
        uint16_t offset=0;
        for (auto &label : this->domain_name)
        {
            offset += label.to_buffer(buffer + offset);
        }
        buffer[offset] = '\0';
        offset++;
        uint16_t* buffer_ptr = reinterpret_cast<uint16_t*>(buffer + offset);
        buffer_ptr[0] = htons(this->question_type);
        buffer_ptr[1] = htons(this->question_class);
        return offset + 4;
    }

    std::string Question::to_string()
    {
        std::string result = "";
        for (auto &label : this->domain_name)
        {
            result += std::string(label.name, label.length);
            result += " ";
        }
        return result;
    }

}