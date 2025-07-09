#include "lable.hpp"

namespace DNS
{

    uint16_t Label::from_buffer(const buffer_t buffer)
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

    uint16_t Label::to_buffer(buffer_t buffer)
    {
        buffer[0] = this->length;
        memcpy(buffer + 1, this->name, this->length);
        return this->length +1;
    }
}
