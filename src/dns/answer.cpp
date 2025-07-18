#include "answer.hpp"

namespace DNS
{

    uint16_t Answer::from_buffer(const buffer_t buffer, uint16_t offset)
    {
        offset = Label::list_from_buffer(buffer, this->domain_name, offset);
        const uint16_t* buffer_short_ptr = reinterpret_cast<const uint16_t*>(buffer + offset);
        this->domain_type = ntohs(buffer_short_ptr[0]);
        this->domain_class = ntohs(buffer_short_ptr[1]);
        offset += 2*sizeof(uint16_t);
        const uint32_t* buffer_int_ptr = reinterpret_cast<const uint32_t*>(buffer + offset);
        this->ttl = ntohl(buffer_int_ptr[0]);
        offset += sizeof(uint32_t);
        const uint16_t* buffer_short_ptr2 = reinterpret_cast<const uint16_t*>(buffer + offset);
        this->data_length = ntohs(buffer_short_ptr2[0]);
        offset += sizeof(uint16_t);
        this->data = buffer + offset;
        offset += this->data_length;
        return offset;
    }
    uint16_t Answer::to_buffer(buffer_t buffer, uint16_t offset)
    {
        offset = Label::list_to_buffer(this->domain_name, buffer, offset);
        uint16_t* buffer_short_ptr = reinterpret_cast<uint16_t*>(buffer + offset);
        buffer_short_ptr[0] = htons(this->domain_type);
        buffer_short_ptr[1] = htons(this->domain_class);
        offset += 2*sizeof(uint16_t);
        uint32_t* buffer_int_ptr = reinterpret_cast<uint32_t*>(buffer + offset);
        buffer_int_ptr[0] = htonl(this->ttl);
        offset += sizeof(uint32_t);
        uint16_t* buffer_short_ptr2 = reinterpret_cast<uint16_t*>(buffer + offset);
        buffer_short_ptr2[0] = htons(this->data_length);
        offset += sizeof(uint16_t);
        memcpy(buffer + offset, this->data, this->data_length);
        offset += this->data_length;
        return offset;
    }
    std::string Answer::to_string()
    {
        std::string result = "";
        result += Label::list_to_string(this->domain_name);
        result +=  std::to_string(this->domain_type) + " " + std::to_string(this->domain_class);
        result +=  " " + std::to_string(this->ttl) + " " + std::to_string(this->data_length);
        return result;
    }
}