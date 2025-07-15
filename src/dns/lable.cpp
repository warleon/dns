#include "lable.hpp"
#include "header.hpp"

namespace DNS
{

    uint16_t Label::from_buffer(const buffer_t buffer, uint16_t offset)
    {
        if (buffer[offset] == '\0') // end of labels
        {
            this->length = 0;
            this->name = nullptr;
            return offset + 1;
        }
        if(buffer[offset] == 0xc0) // pointer
        {
            this->length = 0;
            this->name = nullptr; // TODO: implement pointer
            return offset + 2;
        }
        this->length = buffer[offset];
        this->name = buffer + offset + 1;
        return offset + this->length + 1;
    }

    uint16_t Label::to_buffer(buffer_t buffer, uint16_t offset) const
    {
        buffer[offset] = this->length;
        memcpy(buffer + offset + 1, this->name, this->length);
        return this->length + 1;
    }

    uint16_t Label::list_from_buffer(const buffer_t buffer, list_t &labels, uint16_t offset)
    {
        for (int i = 0; i < buffer_size - offset - sizeof(Header); i++)
        {
            Label label;
            uint16_t new_offset = label.from_buffer(buffer, offset);
            if (new_offset == offset + 1)
            {
                return new_offset;
            }
            offset = new_offset;
            labels.push_back(label);
        }
        return offset;
    }
    uint16_t Label::list_to_buffer(const list_t &labels, buffer_t buffer, uint16_t offset)
    {
        for (auto &label : labels)
        {
            offset = label.to_buffer(buffer, offset);
        }
        buffer[offset] = '\0';
        offset++;
        return offset;
    }
    std::string Label::list_to_string(const list_t &labels)
    {
        std::string result = "";
        for (auto &label : labels)
        {
            result += std::string(reinterpret_cast<char*>(label.name), label.length);
            result += " ";
        }
        return result;
    }
}
