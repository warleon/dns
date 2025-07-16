#include <gtest/gtest.h>
#include "dns/lable.hpp"
#include <vector>
#include <cstring>

class DNSLabelTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
        memset(test_buffer, 0, sizeof(test_buffer));
    }
    
    void TearDown() override {
        // Clean up
    }
    
    uint8_t test_buffer[512];
    
    // Helper function to create a simple domain name in buffer
    void create_domain_in_buffer(const std::vector<std::string>& labels, uint16_t offset = 0) {
        uint16_t current_offset = offset;
        for (const auto& label : labels) {
            test_buffer[current_offset] = static_cast<uint8_t>(label.length());
            memcpy(test_buffer + current_offset + 1, label.c_str(), label.length());
            current_offset += label.length() + 1;
        }
        test_buffer[current_offset] = 0; // null terminator
    }
};

// Test basic Label initialization and default values
TEST_F(DNSLabelTest, DefaultLabelInitialization) {
    DNS::Label label{};
    
    // Test default values (should be uninitialized, but we can test after assignment)
    label.length = 0;
    label.name = nullptr;
    
    EXPECT_EQ(0, label.length);
    EXPECT_EQ(nullptr, label.name);
}

// Test parsing a single label from buffer
TEST_F(DNSLabelTest, SingleLabelFromBuffer) {
    // Create a buffer with "www" (length 3)
    test_buffer[0] = 3;  // length
    test_buffer[1] = 'w';
    test_buffer[2] = 'w';
    test_buffer[3] = 'w';
    
    DNS::Label label;
    uint16_t new_offset = label.from_buffer(test_buffer, 0);
    
    EXPECT_EQ(3, label.length);
    EXPECT_EQ('w', label.name[0]);
    EXPECT_EQ('w', label.name[1]);
    EXPECT_EQ('w', label.name[2]);
    EXPECT_EQ(4, new_offset); // should advance by length + 1
}

// Test parsing null-terminated label (end of labels)
TEST_F(DNSLabelTest, NullTerminatedLabel) {
    // Create a buffer with null terminator
    test_buffer[0] = 0;  // null terminator
    
    DNS::Label label;
    uint16_t new_offset = label.from_buffer(test_buffer, 0);
    
    EXPECT_EQ(0, label.length);
    EXPECT_EQ(nullptr, label.name);
    EXPECT_EQ(1, new_offset); // should advance by 1
}

// Test parsing label with pointer compression
TEST_F(DNSLabelTest, PointerCompressionLabel) {
    // Create a buffer with "example" at offset 10
    test_buffer[10] = 7;  // length
    memcpy(test_buffer + 11, "example", 7);
    test_buffer[18] = 0;  // null terminator
    
    // Create pointer at offset 0 pointing to offset 10
    test_buffer[0] = 0xc0;  // pointer marker
    test_buffer[1] = 10;    // pointer to offset 10
    
    DNS::Label label;
    uint16_t new_offset = label.from_buffer(test_buffer, 0);
    
    EXPECT_EQ(7, label.length);
    EXPECT_EQ('e', label.name[0]);
    EXPECT_EQ('x', label.name[1]);
    EXPECT_EQ('a', label.name[2]);
    EXPECT_EQ('m', label.name[3]);
    EXPECT_EQ('p', label.name[4]);
    EXPECT_EQ('l', label.name[5]);
    EXPECT_EQ('e', label.name[6]);
    EXPECT_EQ(2, new_offset); // pointer takes 2 bytes
}

// Test writing a label to buffer
TEST_F(DNSLabelTest, LabelToBuffer) {
    // Create a label
    DNS::Label label;
    label.length = 3;
    const char* test_name = "www";
    label.name = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(test_name));
    
    uint16_t new_offset = label.to_buffer(test_buffer, 0);
    
    EXPECT_EQ(3, test_buffer[0]);     // length
    EXPECT_EQ('w', test_buffer[1]);   // first char
    EXPECT_EQ('w', test_buffer[2]);   // second char
    EXPECT_EQ('w', test_buffer[3]);   // third char
    EXPECT_EQ(4, new_offset);         // should advance by length + 1
}

// Test round-trip: from_buffer -> to_buffer
TEST_F(DNSLabelTest, RoundTripSingleLabel) {
    // Original data
    test_buffer[0] = 7;
    memcpy(test_buffer + 1, "example", 7);
    
    // Parse from buffer
    DNS::Label label;
    uint16_t parse_offset = label.from_buffer(test_buffer, 0);
    
    // Clear buffer and write back
    memset(test_buffer, 0, sizeof(test_buffer));
    uint16_t write_offset = label.to_buffer(test_buffer, 0);
    
    // Verify
    EXPECT_EQ(parse_offset, write_offset);
    EXPECT_EQ(7, test_buffer[0]);
    EXPECT_EQ(0, memcmp(test_buffer + 1, "example", 7));
}

// Test parsing multiple labels from buffer (domain name)
TEST_F(DNSLabelTest, ListFromBufferSimpleDomain) {
    // Create "www.example.com" in buffer
    create_domain_in_buffer({"www", "example", "com"});
    
    DNS::Label::list_t labels;
    uint16_t new_offset = DNS::Label::list_from_buffer(test_buffer, labels, 0);
    
    ASSERT_EQ(3, labels.size());
    
    auto it = labels.begin();
    
    // Check "www"
    EXPECT_EQ(3, it->length);
    EXPECT_EQ(0, memcmp(it->name, "www", 3));
    ++it;
    
    // Check "example"
    EXPECT_EQ(7, it->length);
    EXPECT_EQ(0, memcmp(it->name, "example", 7));
    ++it;
    
    // Check "com"
    EXPECT_EQ(3, it->length);
    EXPECT_EQ(0, memcmp(it->name, "com", 3));
    
    // Check offset advanced correctly
    EXPECT_EQ(16, new_offset); // 3+1 + 7+1 + 3+1 + 1 = 16
}

// Test writing multiple labels to buffer
TEST_F(DNSLabelTest, ListToBufferSimpleDomain) {
    // Create label list for "test.domain"
    DNS::Label::list_t labels;
    
    DNS::Label label1;
    label1.length = 4;
    const char* name1 = "test";
    label1.name = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(name1));
    labels.push_back(label1);
    
    DNS::Label label2;
    label2.length = 6;
    const char* name2 = "domain";
    label2.name = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(name2));
    labels.push_back(label2);
    
    uint16_t new_offset = DNS::Label::list_to_buffer(labels, test_buffer, 0);
    
    // Verify buffer contents
    EXPECT_EQ(4, test_buffer[0]);           // length of "test"
    EXPECT_EQ(0, memcmp(test_buffer + 1, "test", 4));
    EXPECT_EQ(6, test_buffer[5]);           // length of "domain"
    EXPECT_EQ(0, memcmp(test_buffer + 6, "domain", 6));
    EXPECT_EQ(0, test_buffer[12]);          // null terminator
    EXPECT_EQ(13, new_offset);              // 4+1 + 6+1 + 1 = 13
}

// Test round-trip for label lists
TEST_F(DNSLabelTest, RoundTripLabelList) {
    // Create original domain "hello.world"
    create_domain_in_buffer({"hello", "world"});
    
    // Parse from buffer
    DNS::Label::list_t labels;
    uint16_t parse_offset = DNS::Label::list_from_buffer(test_buffer, labels, 0);
    
    // Clear buffer and write back
    memset(test_buffer, 0, sizeof(test_buffer));
    uint16_t write_offset = DNS::Label::list_to_buffer(labels, test_buffer, 0);
    
    // Parse again to verify
    DNS::Label::list_t labels2;
    uint16_t parse_offset2 = DNS::Label::list_from_buffer(test_buffer, labels2, 0);
    
    EXPECT_EQ(parse_offset, write_offset);
    EXPECT_EQ(parse_offset, parse_offset2);
    EXPECT_EQ(labels.size(), labels2.size());
    
    auto it1 = labels.begin();
    auto it2 = labels2.begin();
    
    while (it1 != labels.end() && it2 != labels2.end()) {
        EXPECT_EQ(it1->length, it2->length);
        EXPECT_EQ(0, memcmp(it1->name, it2->name, it1->length));
        ++it1;
        ++it2;
    }
}

// Test converting label list to string
TEST_F(DNSLabelTest, ListToString) {
    // Create label list for "www.example.com"
    DNS::Label::list_t labels;
    
    DNS::Label label1;
    label1.length = 3;
    const char* name1 = "www";
    label1.name = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(name1));
    labels.push_back(label1);
    
    DNS::Label label2;
    label2.length = 7;
    const char* name2 = "example";
    label2.name = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(name2));
    labels.push_back(label2);
    
    DNS::Label label3;
    label3.length = 3;
    const char* name3 = "com";
    label3.name = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(name3));
    labels.push_back(label3);
    
    std::string result = DNS::Label::list_to_string(labels);
    EXPECT_EQ("www example com ", result);
}

// Test empty label list
TEST_F(DNSLabelTest, EmptyLabelList) {
    DNS::Label::list_t labels;
    
    uint16_t write_offset = DNS::Label::list_to_buffer(labels, test_buffer, 0);
    EXPECT_EQ(0, test_buffer[0]);  // should just write null terminator
    EXPECT_EQ(1, write_offset);
    
    std::string result = DNS::Label::list_to_string(labels);
    EXPECT_EQ("", result);
}

// Test parsing empty label (just null terminator)
TEST_F(DNSLabelTest, EmptyDomain) {
    test_buffer[0] = 0;  // just null terminator
    
    DNS::Label::list_t labels;
    uint16_t new_offset = DNS::Label::list_from_buffer(test_buffer, labels, 0);
    
    EXPECT_EQ(0, labels.size());
    EXPECT_EQ(1, new_offset);
}

// Test maximum length label (63 characters - DNS limit)
TEST_F(DNSLabelTest, MaximumLengthLabel) {
    // Create 63-character label
    test_buffer[0] = 63;
    for (int i = 0; i < 63; i++) {
        test_buffer[i + 1] = 'a' + (i % 26);  // fill with alphabet
    }
    
    DNS::Label label;
    uint16_t new_offset = label.from_buffer(test_buffer, 0);
    
    EXPECT_EQ(63, label.length);
    EXPECT_EQ(64, new_offset);  // 63 + 1
    
    // Verify content
    for (int i = 0; i < 63; i++) {
        EXPECT_EQ('a' + (i % 26), label.name[i]);
    }
}

// Test complex domain with longer labels
TEST_F(DNSLabelTest, ComplexDomain) {
    // Create "subdomain.verylongdomainname.organization.com"
    create_domain_in_buffer({"subdomain", "verylongdomainname", "organization", "com"});
    
    DNS::Label::list_t labels;
    uint16_t new_offset = DNS::Label::list_from_buffer(test_buffer, labels, 0);
    
    ASSERT_EQ(4, labels.size());
    
    auto it = labels.begin();
    
    // Check "subdomain"
    EXPECT_EQ(9, it->length);
    EXPECT_EQ(0, memcmp(it->name, "subdomain", 9));
    ++it;
    
    // Check "verylongdomainname"
    EXPECT_EQ(18, it->length);
    EXPECT_EQ(0, memcmp(it->name, "verylongdomainname", 18));
    ++it;
    
    // Check "organization"
    EXPECT_EQ(12, it->length);
    EXPECT_EQ(0, memcmp(it->name, "organization", 12));
    ++it;
    
    // Check "com"
    EXPECT_EQ(3, it->length);
    EXPECT_EQ(0, memcmp(it->name, "com", 3));
}

// Test starting from non-zero offset
TEST_F(DNSLabelTest, NonZeroOffset) {
    // Put some data at the beginning
    test_buffer[0] = 0xFF;
    test_buffer[1] = 0xFF;
    test_buffer[2] = 0xFF;
    
    // Create "test" starting at offset 3
    test_buffer[3] = 4;
    memcpy(test_buffer + 4, "test", 4);
    test_buffer[8] = 0;  // null terminator
    
    DNS::Label::list_t labels;
    uint16_t new_offset = DNS::Label::list_from_buffer(test_buffer, labels, 3);
    
    ASSERT_EQ(1, labels.size());
    EXPECT_EQ(4, labels.front().length);
    EXPECT_EQ(0, memcmp(labels.front().name, "test", 4));
    EXPECT_EQ(9, new_offset);  // 3 + 4 + 1 + 1 = 9
}

// Test error handling for buffer bounds (basic safety check)
TEST_F(DNSLabelTest, BufferBoundsBasicCheck) {
    // This test ensures we don't crash with reasonable invalid input
    // Note: The current implementation doesn't have extensive bounds checking
    // This is a basic safety test
    
    // Fill buffer with reasonable data that won't cause obvious crashes
    test_buffer[0] = 5;  // length 5
    memcpy(test_buffer + 1, "valid", 5);
    test_buffer[6] = 0;  // null terminator
    
    DNS::Label label;
    uint16_t new_offset = label.from_buffer(test_buffer, 0);
    
    EXPECT_EQ(5, label.length);
    EXPECT_EQ(0, memcmp(label.name, "valid", 5));
    EXPECT_EQ(7, new_offset);
}

// Test single character labels
TEST_F(DNSLabelTest, SingleCharacterLabels) {
    // Create "a.b.c"
    create_domain_in_buffer({"a", "b", "c"});
    
    DNS::Label::list_t labels;
    uint16_t new_offset = DNS::Label::list_from_buffer(test_buffer, labels, 0);
    
    ASSERT_EQ(3, labels.size());
    
    auto it = labels.begin();
    EXPECT_EQ(1, it->length);
    EXPECT_EQ('a', it->name[0]);
    ++it;
    
    EXPECT_EQ(1, it->length);
    EXPECT_EQ('b', it->name[0]);
    ++it;
    
    EXPECT_EQ(1, it->length);
    EXPECT_EQ('c', it->name[0]);
    
    EXPECT_EQ(7, new_offset);  // 1+1 + 1+1 + 1+1 + 1 = 7
}
