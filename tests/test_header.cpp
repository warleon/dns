#include <gtest/gtest.h>
#include "dns/header.hpp"
#include <vector>
#include <cstring>

class DNSHeaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
        memset(test_buffer, 0, sizeof(test_buffer));
    }
    
    void TearDown() override {
        // Clean up
    }
    
    uint8_t test_buffer[512];
};

TEST_F(DNSHeaderTest, DefaultHeaderInitialization) {
    DNS::Header header{};
    
    // Test default values
    EXPECT_EQ(0, header.id);
    EXPECT_EQ(0, header.query_response);
    EXPECT_EQ(0, header.opcode);
    EXPECT_EQ(0, header.authoritative_answer);
    EXPECT_EQ(0, header.truncated_message);
    EXPECT_EQ(0, header.recursion_desired);
    EXPECT_EQ(0, header.recursion_available);
    EXPECT_EQ(0, header.reserved);
    EXPECT_EQ(0, header.authentic_data);
    EXPECT_EQ(0, header.checking_disabled);
    EXPECT_EQ(0, header.response_code);
    EXPECT_EQ(0, header.question_count);
    EXPECT_EQ(0, header.answer_count);
    EXPECT_EQ(0, header.authority_count);
    EXPECT_EQ(0, header.additional_count);
}

TEST_F(DNSHeaderTest, FieldAssignment) {
    DNS::Header header{};
    
    // Test setting each field individually
    header.id = 0x1234;
    header.query_response = 1;
    header.opcode = 5;
    header.authoritative_answer = 1;
    header.truncated_message = 1;
    header.recursion_desired = 1;
    header.recursion_available = 1;
    header.reserved = 1;
    header.authentic_data = 1;
    header.checking_disabled = 1;
    header.response_code = 7;
    header.question_count = 10;
    header.answer_count = 20;
    header.authority_count = 30;
    header.additional_count = 40;
    
    // Verify all fields are set correctly
    EXPECT_EQ(0x1234, header.id);
    EXPECT_EQ(1, header.query_response);
    EXPECT_EQ(5, header.opcode);
    EXPECT_EQ(1, header.authoritative_answer);
    EXPECT_EQ(1, header.truncated_message);
    EXPECT_EQ(1, header.recursion_desired);
    EXPECT_EQ(1, header.recursion_available);
    EXPECT_EQ(1, header.reserved);
    EXPECT_EQ(1, header.authentic_data);
    EXPECT_EQ(1, header.checking_disabled);
    EXPECT_EQ(7, header.response_code);
    EXPECT_EQ(10, header.question_count);
    EXPECT_EQ(20, header.answer_count);
    EXPECT_EQ(30, header.authority_count);
    EXPECT_EQ(40, header.additional_count);
}

TEST_F(DNSHeaderTest, BitFieldBoundaries) {
    DNS::Header header{};
    
    // Test maximum values for bit fields
    header.query_response = 1;  // 1 bit max
    header.opcode = 15;         // 4 bits max
    header.authoritative_answer = 1;  // 1 bit max
    header.truncated_message = 1;     // 1 bit max
    header.recursion_desired = 1;     // 1 bit max
    header.recursion_available = 1;   // 1 bit max
    header.reserved = 1;              // 1 bit max
    header.authentic_data = 1;        // 1 bit max
    header.checking_disabled = 1;     // 1 bit max
    header.response_code = 15;        // 4 bits max
    
    EXPECT_EQ(1, header.query_response);
    EXPECT_EQ(15, header.opcode);
    EXPECT_EQ(1, header.authoritative_answer);
    EXPECT_EQ(1, header.truncated_message);
    EXPECT_EQ(1, header.recursion_desired);
    EXPECT_EQ(1, header.recursion_available);
    EXPECT_EQ(1, header.reserved);
    EXPECT_EQ(1, header.authentic_data);
    EXPECT_EQ(1, header.checking_disabled);
    EXPECT_EQ(15, header.response_code);
}

TEST_F(DNSHeaderTest, ToBufferBasic) {
    DNS::Header header{};
    header.id = 0x1234;
    header.query_response = 1;
    header.opcode = 2;
    header.authoritative_answer = 1;
    header.truncated_message = 0;
    header.recursion_desired = 1;
    header.recursion_available = 1;
    header.reserved = 0;
    header.authentic_data = 1;
    header.checking_disabled = 0;
    header.response_code = 3;
    header.question_count = 1;
    header.answer_count = 2;
    header.authority_count = 3;
    header.additional_count = 4;
    
    uint16_t offset = header.to_buffer(test_buffer, 0);
    
    // Check return value (should be sizeof(header))
    EXPECT_EQ(12, offset);  // Header is 12 bytes
    
    // Check ID (bytes 0-1, network byte order)
    EXPECT_EQ(0x12, test_buffer[0]);
    EXPECT_EQ(0x34, test_buffer[1]);
    
    // Check flags byte 2: QR(1) + OPCODE(2) + AA(1) + TC(0) + RD(1) = 10010101 = 0x95
    EXPECT_EQ(0x95, test_buffer[2]);
    
    // Check flags byte 3: RA(1) + Z(0) + AD(1) + CD(0) + RCODE(3) = 10100011 = 0xA3
    EXPECT_EQ(0xA3, test_buffer[3]);
    
    // Check counts (bytes 4-11, network byte order)
    EXPECT_EQ(0x00, test_buffer[4]);  // question_count high byte
    EXPECT_EQ(0x01, test_buffer[5]);  // question_count low byte
    EXPECT_EQ(0x00, test_buffer[6]);  // answer_count high byte
    EXPECT_EQ(0x02, test_buffer[7]);  // answer_count low byte
    EXPECT_EQ(0x00, test_buffer[8]);  // authority_count high byte
    EXPECT_EQ(0x03, test_buffer[9]);  // authority_count low byte
    EXPECT_EQ(0x00, test_buffer[10]); // additional_count high byte
    EXPECT_EQ(0x04, test_buffer[11]); // additional_count low byte
}

TEST_F(DNSHeaderTest, FromBufferBasic) {
    // Set up a known buffer
    test_buffer[0] = 0x12;  // ID high byte
    test_buffer[1] = 0x34;  // ID low byte
    test_buffer[2] = 0x95;  // Flags: QR=1, OPCODE=2, AA=1, TC=0, RD=1
    test_buffer[3] = 0xA3;  // Flags: RA=1, Z=0, AD=1, CD=0, RCODE=3
    test_buffer[4] = 0x00;  // question_count high byte
    test_buffer[5] = 0x01;  // question_count low byte
    test_buffer[6] = 0x00;  // answer_count high byte
    test_buffer[7] = 0x02;  // answer_count low byte
    test_buffer[8] = 0x00;  // authority_count high byte
    test_buffer[9] = 0x03;  // authority_count low byte
    test_buffer[10] = 0x00; // additional_count high byte
    test_buffer[11] = 0x04; // additional_count low byte
    
    DNS::Header header{};
    uint16_t offset = header.from_buffer(test_buffer, 0);
    
    // Check return value
    EXPECT_EQ(12, offset);
    
    // Check all fields
    EXPECT_EQ(0x1234, header.id);
    EXPECT_EQ(1, header.query_response);
    EXPECT_EQ(2, header.opcode);
    EXPECT_EQ(1, header.authoritative_answer);
    EXPECT_EQ(0, header.truncated_message);
    EXPECT_EQ(1, header.recursion_desired);
    EXPECT_EQ(1, header.recursion_available);
    EXPECT_EQ(0, header.reserved);
    EXPECT_EQ(1, header.authentic_data);
    EXPECT_EQ(0, header.checking_disabled);
    EXPECT_EQ(3, header.response_code);
    EXPECT_EQ(1, header.question_count);
    EXPECT_EQ(2, header.answer_count);
    EXPECT_EQ(3, header.authority_count);
    EXPECT_EQ(4, header.additional_count);
}

TEST_F(DNSHeaderTest, RoundTripConversion) {
    DNS::Header original{};
    original.id = 0xABCD;
    original.query_response = 1;
    original.opcode = 4;
    original.authoritative_answer = 1;
    original.truncated_message = 1;
    original.recursion_desired = 0;
    original.recursion_available = 1;
    original.reserved = 0;
    original.authentic_data = 1;
    original.checking_disabled = 1;
    original.response_code = 5;
    original.question_count = 100;
    original.answer_count = 200;
    original.authority_count = 300;
    original.additional_count = 400;
    
    // Convert to buffer
    original.to_buffer(test_buffer, 0);
    
    // Convert back from buffer
    DNS::Header reconstructed{};
    reconstructed.from_buffer(test_buffer, 0);
    
    // Check that all fields match
    EXPECT_EQ(original.id, reconstructed.id);
    EXPECT_EQ(original.query_response, reconstructed.query_response);
    EXPECT_EQ(original.opcode, reconstructed.opcode);
    EXPECT_EQ(original.authoritative_answer, reconstructed.authoritative_answer);
    EXPECT_EQ(original.truncated_message, reconstructed.truncated_message);
    EXPECT_EQ(original.recursion_desired, reconstructed.recursion_desired);
    EXPECT_EQ(original.recursion_available, reconstructed.recursion_available);
    EXPECT_EQ(original.reserved, reconstructed.reserved);
    EXPECT_EQ(original.authentic_data, reconstructed.authentic_data);
    EXPECT_EQ(original.checking_disabled, reconstructed.checking_disabled);
    EXPECT_EQ(original.response_code, reconstructed.response_code);
    EXPECT_EQ(original.question_count, reconstructed.question_count);
    EXPECT_EQ(original.answer_count, reconstructed.answer_count);
    EXPECT_EQ(original.authority_count, reconstructed.authority_count);
    EXPECT_EQ(original.additional_count, reconstructed.additional_count);
}

TEST_F(DNSHeaderTest, WithOffset) {
    DNS::Header header{};
    header.id = 0x5678;
    header.question_count = 42;
    
    // Write header at offset 10
    uint16_t write_offset = header.to_buffer(test_buffer, 10);
    EXPECT_EQ(22, write_offset);  // 10 + 12
    
    // Read header from offset 10
    DNS::Header read_header{};
    uint16_t read_offset = read_header.from_buffer(test_buffer, 10);
    EXPECT_EQ(22, read_offset);  // 10 + 12
    
    EXPECT_EQ(0x5678, read_header.id);
    EXPECT_EQ(42, read_header.question_count);
}

TEST_F(DNSHeaderTest, AllFlagsSet) {
    DNS::Header header{};
    header.query_response = 1;
    header.opcode = 15;  // max value for 4 bits
    header.authoritative_answer = 1;
    header.truncated_message = 1;
    header.recursion_desired = 1;
    header.recursion_available = 1;
    header.reserved = 1;
    header.authentic_data = 1;
    header.checking_disabled = 1;
    header.response_code = 15;  // max value for 4 bits
    
    header.to_buffer(test_buffer, 0);
    
    DNS::Header reconstructed{};
    reconstructed.from_buffer(test_buffer, 0);
    
    EXPECT_EQ(1, reconstructed.query_response);
    EXPECT_EQ(15, reconstructed.opcode);
    EXPECT_EQ(1, reconstructed.authoritative_answer);
    EXPECT_EQ(1, reconstructed.truncated_message);
    EXPECT_EQ(1, reconstructed.recursion_desired);
    EXPECT_EQ(1, reconstructed.recursion_available);
    EXPECT_EQ(1, reconstructed.reserved);
    EXPECT_EQ(1, reconstructed.authentic_data);
    EXPECT_EQ(1, reconstructed.checking_disabled);
    EXPECT_EQ(15, reconstructed.response_code);
}

TEST_F(DNSHeaderTest, AllFlagsClear) {
    DNS::Header header{};
    // All flags should already be 0 from default initialization
    
    header.to_buffer(test_buffer, 0);
    
    DNS::Header reconstructed{};
    reconstructed.from_buffer(test_buffer, 0);
    
    EXPECT_EQ(0, reconstructed.query_response);
    EXPECT_EQ(0, reconstructed.opcode);
    EXPECT_EQ(0, reconstructed.authoritative_answer);
    EXPECT_EQ(0, reconstructed.truncated_message);
    EXPECT_EQ(0, reconstructed.recursion_desired);
    EXPECT_EQ(0, reconstructed.recursion_available);
    EXPECT_EQ(0, reconstructed.reserved);
    EXPECT_EQ(0, reconstructed.authentic_data);
    EXPECT_EQ(0, reconstructed.checking_disabled);
    EXPECT_EQ(0, reconstructed.response_code);
}

TEST_F(DNSHeaderTest, MaximumCounts) {
    DNS::Header header{};
    header.question_count = 0xFFFF;    // Maximum uint16_t value
    header.answer_count = 0xFFFF;
    header.authority_count = 0xFFFF;
    header.additional_count = 0xFFFF;
    
    header.to_buffer(test_buffer, 0);
    
    DNS::Header reconstructed{};
    reconstructed.from_buffer(test_buffer, 0);
    
    EXPECT_EQ(0xFFFF, reconstructed.question_count);
    EXPECT_EQ(0xFFFF, reconstructed.answer_count);
    EXPECT_EQ(0xFFFF, reconstructed.authority_count);
    EXPECT_EQ(0xFFFF, reconstructed.additional_count);
}
