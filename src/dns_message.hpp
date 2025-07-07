#pragma once

#include <cstdint>
#include <bit>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include "types.hpp"

// Offset	Octet	|0	                            |1	                            |2	                            |3
// Octet	    Bit 	|0	|1	|2	|3	|4	|5	|6	|7	|8	|9	|10	|11	|12	|13	|14	|15	|16	|17	|18	|19	|20	|21	|22	|23	|24	|25	|26	|27	|28	|29	|30	|31 |
// 0	0           	|Transaction ID                                                 |QR	|OPCODE	        |AA	|TC	|RD	|RA	|Z	|AD	|CD	|RCODE          |
// 4	32	            |Number of Questions	                                        |Number of Answers	                                            |
// 8 64              |Number of Authority RRs	                                    |Number of additional RRs                                       |
struct dns_header
{
    uint16_t id;                       // 0-15
    uint16_t query_response : 1;       // 16
    uint16_t opcode : 4;               // 17-20
    uint16_t authoritative_answer : 1; // 21
    uint16_t truncated_message : 1;    // 22
    uint16_t recursion_desired : 1;    // 23
    uint16_t recursion_available : 1;  // 24
    uint16_t reserved : 1;             // 25
    uint16_t authentic_data : 1;       // 26
    uint16_t checking_disabled : 1;    // 27
    uint16_t response_code : 4;        // 28-31
    uint16_t question_count;           // 32-47
    uint16_t answer_count;             // 48-63
    uint16_t authority_count;          // 64-79
    uint16_t additional_count;         // 80-95

    uint16_t from_buffer(const buffer_t buffer)
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

    uint16_t to_buffer(buffer_t buffer)
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
};

struct dns_domain_name_label
{
    uint8_t length;
    char *name;

    uint16_t from_buffer(const buffer_t buffer)
    {
        if (buffer[0] == '\0')
        {
            this->length = 0;
            this->name = nullptr;
            return 0;
        }
        this->length = buffer[0];
        this->name = buffer + 1;
        return this->length + 1;
    }

    uint16_t to_buffer(buffer_t buffer)
    {
        buffer[0] = this->length;
        memcpy(buffer + 1, this->name, this->length);
        return this->length +1;
    }
};

struct dns_question
{
    std::list<dns_domain_name_label> domain_name;
    uint16_t question_type;
    uint16_t question_class;

    uint16_t from_buffer(const buffer_t buffer)
    {
        uint16_t offset = 0;
        for (int i = 0; i < buffer_size - sizeof(dns_header); i++)
        {
            dns_domain_name_label label;
            uint16_t label_size = label.from_buffer(buffer + offset);
            if (label_size == 0)
            {
                offset++;
                break;
            }
            offset += label_size;
            this->domain_name.push_back(label);
        }
        this->question_type = ntohs(uint16_t(buffer[offset]));
        this->question_class = ntohs(uint16_t(buffer[offset + 2]));
        return offset + 4;
    }

    uint16_t to_buffer(buffer_t buffer)
    {
        uint16_t offset=0;
        for (auto &label : this->domain_name)
        {
            offset += label.to_buffer(buffer + offset);
        }
        buffer[offset] = '\0';
        offset++;
        uint16_t &question_type = *(uint16_t *)&buffer[offset];
        question_type = htons(this->question_type);
        uint16_t &question_class = *(uint16_t *)&buffer[offset + 2];
        question_class = htons(this->question_class);
        return offset + 4;
    }
};

class DNSMessage
{
    dns_header header = {};
    std::vector<dns_question> questions = {};
public:
    DNSMessage() {}
    DNSMessage(buffer_t buffer)
    {
        uint16_t offset = 0;
        offset += this->header.from_buffer(buffer);
        questions.reserve(this->header.question_count);
        for (int i = 0; i < this->header.question_count; i++)
        {
            dns_question question;
            offset += question.from_buffer(buffer + offset);
            questions.emplace_back(question);
        }
    }
    DNSMessage(DNSMessage &query)
    {
        this->from_question(query);
    }
    ~DNSMessage() {}

    void from_question(DNSMessage &query)
    {
        this->header.id = query.header.id;
        this->header.query_response = 1;
        // all 0 for the moment
        // this->header.opcode = query.header.opcode;
        // this->header.authoritative_answer = query.header.authoritative_answer;
        // this->header.truncated_message = query.header.truncated_message;
        // this->header.recursion_desired = query.header.recursion_desired;
        // this->header.recursion_available = query.header.recursion_available;
        // this->header.reserved = query.header.reserved;
    }
    uint16_t serialize(buffer_t buffer)
    {
        uint16_t offset = 0;
        offset += this->header.to_buffer(buffer);
        for (auto &question : this->questions)
        {
            offset += question.to_buffer(buffer + offset);
        }
        return offset;
    }
};