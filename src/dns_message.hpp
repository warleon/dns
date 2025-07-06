#pragma once

#include <cstdint>
#include <bit>
#include "types.hpp"

struct dns_header {
    uint16_t id;
    uint8_t query_response: 1;
    uint8_t opcode: 4;
    uint8_t authoritative_answer: 1;
    uint8_t truncated_message: 1;
    uint8_t recursion_desired: 1;
    uint8_t recursion_available: 1;
    uint8_t reserved: 3;
    uint8_t response_code: 4;
    uint16_t question_count;
    uint16_t answer_count;
    uint16_t authority_count;
    uint16_t additional_count;
};

void n2h_memcpy(void* dst, const void* src, size_t n){
    if ( std::endian::native == std::endian::big){
        memcpy(dst, src, n);
    } else {
        for (size_t i = 0; i < n; i++) {
            ((uint8_t*)dst)[i] = ((const uint8_t*)src)[n-i-1];
        }
    }
}
void h2n_memcpy(void* dst, const void* src, size_t n){
    if ( std::endian::native == std::endian::big){
        memcpy(dst, src, n);
    } else {
        for (size_t i = 0; i < n; i++) {
            ((uint8_t*)dst)[i] = ((const uint8_t*)src)[n-i-1];
        }
    }
}

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
            h2n_memcpy(&this->header, buffer, sizeof(dns_header));
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
            n2h_memcpy(buffer, &this->header, sizeof(this->header));
            return sizeof(this->header);
        }
};