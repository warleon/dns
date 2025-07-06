#pragma once

#include <cstdint>
#include <bit>
#include "types.hpp"

struct dns_header {
    uint16_t id;
    union {
        uint16_t flags;
        struct {
            uint16_t query_response: 1;
            uint16_t opcode: 4;
            uint16_t authoritative_answer: 1;
            uint16_t truncated_message: 1;
            uint16_t recursion_desired: 1;
            uint16_t recursion_available: 1;
            uint16_t reserved: 3;
            uint16_t response_code: 4;
        };
    };
    uint16_t question_count;
    uint16_t answer_count;
    uint16_t authority_count;
    uint16_t additional_count;
};

class DNSMessage {
    public:
        dns_header header = {};
        DNSMessage(){}
        DNSMessage(buffer_t& buffer){
            this->from_buffer(buffer);
        }
        DNSMessage(DNSMessage& query){
            this->from_question(query);
        }
        ~DNSMessage(){}
        void from_buffer(buffer_t& buffer){
            const uint16_t* header_ptr = reinterpret_cast<const uint16_t*>(buffer);
            this->header.id = ntohs(header_ptr[0]);
            this->header.flags = ntohs(header_ptr[1]);
            this->header.question_count = ntohs(header_ptr[2]);
            this->header.answer_count = ntohs(header_ptr[3]);
            this->header.authority_count = ntohs(header_ptr[4]);
            this->header.additional_count = ntohs(header_ptr[5]);
        }
        void from_question(DNSMessage& query){
            this->header.id = query.header.id;
            this->header.query_response = 1;
            // all 0 for the moment
            //this->header.opcode = query.header.opcode;
            //this->header.authoritative_answer = query.header.authoritative_answer;
            //this->header.truncated_message = query.header.truncated_message;
            //this->header.recursion_desired = query.header.recursion_desired;
            //this->header.recursion_available = query.header.recursion_available;
            //this->header.reserved = query.header.reserved;
        }
        uint16_t to_buffer(buffer_t& buffer){
            uint16_t* header_ptr = reinterpret_cast<uint16_t*>(buffer);
            header_ptr[0] = htons(this->header.id);
            header_ptr[1] = htons(this->header.flags);
            header_ptr[2] = htons(this->header.question_count);
            header_ptr[3] = htons(this->header.answer_count);
            header_ptr[4] = htons(this->header.authority_count);
            header_ptr[5] = htons(this->header.additional_count);
            return sizeof(this->header);
        }

        void print_header(){
            std::cout << "Header: " << std::endl;
            std::cout << "\tID: " << this->header.id << std::endl;
            std::cout << "\tQuery/Response: " << (this->header.query_response ? "Query" : "Response") << std::endl;
            std::cout << "\tOpcode: " << this->header.opcode << std::endl;
            std::cout << "\tAuthoritative Answer: " << this->header.authoritative_answer << std::endl;
            std::cout << "\tTruncated Message: " << this->header.truncated_message << std::endl;
            std::cout << "\tRecursion Desired: " << this->header.recursion_desired << std::endl;
            std::cout << "\tRecursion Available: " << this->header.recursion_available << std::endl;
            std::cout << "\tReserved: " << this->header.reserved << std::endl;
            std::cout << "\tResponse Code: " << this->header.response_code << std::endl;
            std::cout << "\tQuestion Count: " << this->header.question_count << std::endl;
            std::cout << "\tAnswer Count: " << this->header.answer_count << std::endl;
            std::cout << "\tAuthority Count: " << this->header.authority_count << std::endl;
            std::cout << "\tAdditional Count: " << this->header.additional_count << std::endl;
        }
};