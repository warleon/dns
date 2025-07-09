#include "header.hpp"

namespace DNS
{
    uint16_t Header::from_buffer(const buffer_t buffer)
    {
        const uint16_t *header_ptr = reinterpret_cast<const uint16_t *>(buffer);
        this->id = ntohs(header_ptr[0]);
        {
            this->query_response = (buffer[2] & 0x80) >> 7;
            this->opcode = (buffer[2] & 0x78) >> 3;
            this->authoritative_answer = (buffer[2] & 0x04) >> 2;
            this->truncated_message = (buffer[2] & 0x02) >> 1;
            this->recursion_desired = (buffer[2] & 0x01);
            this->recursion_available = (buffer[3] & 0x80) >> 7;
            this->reserved = (buffer[3] & 0x40) >> 6;
            this->authentic_data = (buffer[3] & 0x20) >> 5;
            this->checking_disabled = (buffer[3] & 0x10) >> 4;
            this->response_code = (buffer[3] & 0x0F);
        }
        this->question_count = ntohs(header_ptr[2]);
        this->answer_count = ntohs(header_ptr[3]);
        this->authority_count = ntohs(header_ptr[4]);
        this->additional_count = ntohs(header_ptr[5]);
        return sizeof(*this);
    }

    uint16_t Header::to_buffer(buffer_t buffer)
    {
        uint16_t *header_ptr = reinterpret_cast<uint16_t *>(buffer);
        header_ptr[0] = htons(this->id);
        {
            buffer[2] = (this->query_response << 7) | (this->opcode << 3) | (this->authoritative_answer << 2) | (this->truncated_message << 1) | this->recursion_desired;
            buffer[3] = (this->recursion_available << 7) | (this->reserved << 6) | (this->authentic_data << 5) | (this->checking_disabled << 4) | this->response_code;
        }
        header_ptr[2] = htons(this->question_count);
        header_ptr[3] = htons(this->answer_count);
        header_ptr[4] = htons(this->authority_count);
        header_ptr[5] = htons(this->additional_count);
        return sizeof(*this);
    }
}