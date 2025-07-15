#pragma once

#include "imports.hpp"
namespace DNS
{

// Offset	Octet	|0	                            |1	                            |2	                            |3
// Octet	    Bit 	|0	|1	|2	|3	|4	|5	|6	|7	|8	|9	|10	|11	|12	|13	|14	|15	|16	|17	|18	|19	|20	|21	|22	|23	|24	|25	|26	|27	|28	|29	|30	|31 |
// 0	0           	|Transaction ID                                                 |QR	|OPCODE	        |AA	|TC	|RD	|RA	|Z	|AD	|CD	|RCODE          |
// 4	32	            |Number of Questions	                                        |Number of Answers	                                            |
// 8 64                 |Number of Authority RRs	                                    |Number of additional RRs                                       |
struct Header
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

    uint16_t from_buffer(const buffer_t buffer, uint16_t offset );
    uint16_t to_buffer(buffer_t buffer, uint16_t offset );
};

}