#include "question.hpp"

namespace DNS
{

    uint16_t Question::from_buffer(const buffer_t buffer, uint16_t offset)
    {
        offset = Label::list_from_buffer(buffer, this->domain_name, offset);
        const uint16_t* buffer_ptr = reinterpret_cast<const uint16_t*>(buffer + offset);
        this->domain_type = ntohs(buffer_ptr[0]);
        this->domain_class = ntohs(buffer_ptr[1]);
        return offset + 2*sizeof(uint16_t);
    }

    uint16_t Question::to_buffer(buffer_t buffer, uint16_t offset)
    {
        offset = Label::list_to_buffer(this->domain_name, buffer, offset);
        uint16_t* buffer_ptr = reinterpret_cast<uint16_t*>(buffer + offset);
        buffer_ptr[0] = htons(this->domain_type);
        buffer_ptr[1] = htons(this->domain_class);
        return offset  + 2*sizeof(uint16_t);
    }

    std::string Question::to_string()
    {
        std::string result = "";
        result += Label::list_to_string(this->domain_name);
        result +=  std::to_string(this->domain_type) + " " + std::to_string(this->domain_class);
        return result;
    }

}