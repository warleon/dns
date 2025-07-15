#include <gtest/gtest.h>
#include "dns/header.hpp"
#include <vector>

class DNSHeaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
    }
    
    void TearDown() override {
        // Clean up
    }
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

// failing test: alwais fails, tests nothing
TEST_F(DNSHeaderTest, FailingTest) {
    FAIL();
}