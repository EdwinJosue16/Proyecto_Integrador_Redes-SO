#ifndef PAYLOAD
#define PAYLOAD

#include <cstdint>
#include "common.hpp"
#include <memory>
#include <vector>
#include <sstream>

/*
 *  Smart pointers and polymorphism.
 *  https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
 *  https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared
 */

const size_t MAX_BUFFER_SIZE = 220;

class Payload
{
public:
    typedef std::shared_ptr<Payload> payload_ptr;
    DISABLE_COPY(Payload);

    uint8_t type;
    payload_ptr payload;

    static const uint8_t MAX_CHARACTERS_IN_ONE_BYTE = 3;

public:
    explicit Payload(payload_ptr payload)
        : payload(payload)
    {
    }

    Payload()
    {
    }
    virtual ~Payload()
    {
    }

    virtual void deserialize(const char *buffer) = 0;
    virtual void serialize(char *buffer) = 0;
    virtual size_t getBytesRepresentationCount() = 0;
    virtual payload_ptr makeDeepCopy() = 0;

    inline virtual uint8_t getType()
    {
        return type;
    }

    inline virtual void setType(uint8_t type)
    {
        this->type = type;
    }

    inline virtual payload_ptr getPayload()
    {
        return payload;
    }

    inline virtual void setPayload(payload_ptr payload)
    {
        this->payload = payload;
    }

    std::shared_ptr<std::vector<std::string>> split(std::string &str, char delimiter)
    {
        auto tokens = std::make_shared<std::vector<std::string>>();
        std::stringstream stringstream(str);
        std::string token;

        while (getline(stringstream, token, delimiter))
        {
            tokens->push_back(token);
        }
        return tokens;
    }

    std::string stringType()
    {
        char deserializedType[MAX_CHARACTERS_IN_ONE_BYTE];
        sprintf(deserializedType, "%02X", this->type);
        return std::string(deserializedType);
    }
};
#endif
