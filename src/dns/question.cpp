#include "question.hpp"

namespace DNS
{

    uint16_t Question::from_buffer(const buffer_t buffer)
    {
        uint16_t offset = 0;
        offset += Label::list_from_buffer(buffer + offset, this->domain_name);
        const uint16_t* buffer_ptr = reinterpret_cast<const uint16_t*>(buffer + offset);
        this->domain_type = ntohs(buffer_ptr[0]);
        this->domain_class = ntohs(buffer_ptr[1]);
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
        buffer_ptr[0] = htons(this->domain_type);
        buffer_ptr[1] = htons(this->domain_class);
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