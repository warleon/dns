#pragma once

#include <cstdint>
#include <bit>
#include <iostream>
#include <bitset>
#include "types.hpp"

//Offset	Octet	|0	                            |1	                            |2	                            |3
//Octet	    Bit 	|0	|1	|2	|3	|4	|5	|6	|7	|8	|9	|10	|11	|12	|13	|14	|15	|16	|17	|18	|19	|20	|21	|22	|23	|24	|25	|26	|27	|28	|29	|30	|31 |
//0	0           	|Transaction ID                                                 |QR	|OPCODE	        |AA	|TC	|RD	|RA	|Z	|AD	|CD	|RCODE          |
//4	32	            |Number of Questions	                                        |Number of Answers	                                            |   
//8 64              |Number of Authority RRs	                                    |Number of additional RRs                                       |
struct dns_header {
    uint16_t id; // 0-15
    union {
        uint16_t flags; // 16-31
        struct {
            uint8_t flags_1; // 16-23
            uint8_t flags_0; // 24-31
        };
        struct {
            uint16_t query_response: 1; // 16
            uint16_t opcode: 4; // 17-20
            uint16_t authoritative_answer: 1; // 21
            uint16_t truncated_message: 1; // 22
            uint16_t recursion_desired: 1; // 23
            uint16_t recursion_available: 1; // 24
            uint16_t reserved: 1; // 25
            uint16_t authentic_data: 1; // 26
            uint16_t checking_disabled: 1; // 27
            uint16_t response_code: 4; // 28-31
        };
    };
    uint16_t question_count; // 32-47
    uint16_t answer_count; // 48-63
    uint16_t authority_count; // 64-79
    uint16_t additional_count; // 80-95
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

        //Offset	Octet	|0	                            |1	                            |2	                            |3
        //Octet	    Bit 	|0	|1	|2	|3	|4	|5	|6	|7	|8	|9	|10	|11	|12	|13	|14	|15	|16	|17	|18	|19	|20	|21	|22	|23	|24	|25	|26	|27	|28	|29	|30	|31 |
        //0	0           	|Transaction ID                                                 |QR	|OPCODE	        |AA	|TC	|RD	|RA	|Z	|AD	|CD	|RCODE          |
        //4	32	            |Number of Questions	                                        |Number of Answers	                                            |   
        //8 64              |Number of Authority RRs	                                    |Number of additional RRs                                       |
        void from_buffer(buffer_t& buffer){
            const uint16_t* header_ptr = reinterpret_cast<const uint16_t*>(buffer);
            this->header.id = ntohs(header_ptr[0]);
            {
                this->header.query_response = (buffer[2] & 0x80) >> 7;
                this->header.opcode = (buffer[2] & 0x78) >> 3;
                this->header.authoritative_answer = (buffer[2] & 0x04) >> 2;
                this->header.truncated_message = (buffer[2] & 0x02) >> 1;
                this->header.recursion_desired = (buffer[2] & 0x01);
                this->header.recursion_available = (buffer[3] & 0x80) >> 7;
                this->header.reserved = (buffer[3] & 0x40) >> 6;
                this->header.authentic_data = (buffer[3] & 0x20) >> 5;
                this->header.checking_disabled = (buffer[3] & 0x10) >> 4;
                this->header.response_code = (buffer[3] & 0x0F);
            }
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

        //Offset	Octet	|0	                            |1	                            |2	                            |3
        //Octet	    Bit 	|0	|1	|2	|3	|4	|5	|6	|7	|8	|9	|10	|11	|12	|13	|14	|15	|16	|17	|18	|19	|20	|21	|22	|23	|24	|25	|26	|27	|28	|29	|30	|31 |
        //0	0           	|Transaction ID                                                 |QR	|OPCODE	        |AA	|TC	|RD	|RA	|Z	|AD	|CD	|RCODE          |
        //4	32	            |Number of Questions	                                        |Number of Answers	                                            |   
        //8 64              |Number of Authority RRs	                                    |Number of additional RRs                                       |
        uint16_t to_buffer(buffer_t& buffer){
            uint16_t* header_ptr = reinterpret_cast<uint16_t*>(buffer);
            header_ptr[0] = htons(this->header.id);
            {
                buffer[2] = (this->header.query_response << 7) | (this->header.opcode << 3) | (this->header.authoritative_answer << 2) | (this->header.truncated_message << 1) | this->header.recursion_desired;
                buffer[3] = (this->header.recursion_available << 7) | (this->header.reserved << 6) | (this->header.authentic_data << 5) | (this->header.checking_disabled << 4) | this->header.response_code;
            }
            header_ptr[2] = htons(this->header.question_count);
            header_ptr[3] = htons(this->header.answer_count);
            header_ptr[4] = htons(this->header.authority_count);
            header_ptr[5] = htons(this->header.additional_count);
            return sizeof(this->header);
        }

        void print_header(){
            std::cout << "Header: " << std::endl;
            std::cout << "\tID: " << this->header.id << std::endl;
            std::cout << "\tQuery/Response: " << (this->header.query_response) << " " << (!this->header.query_response ? "Query" : "Response") << std::endl;
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