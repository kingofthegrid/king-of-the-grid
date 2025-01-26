#include <string>
#include <sstream>
#include <iomanip>
#include "utils.h"

std::string escape_json(const std::string& input)
{
    std::stringstream escaped;

    for (char c : input)
    {
        switch (c)
        {
            case '"':  escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '/':  escaped << "\\/"; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:
                if (c < 0x20)
                {
                    escaped << "\\u"
                            << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                }
                else
                {
                    escaped << c;
                }
                break;
        }
    }

    return escaped.str();
}

std::string escape_str(const std::string& input)
{
    std::stringstream escaped;

    for (char c : input)
    {
        switch (c)
        {
            case '"':
            case '\\':
            case '/':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
            {
                escaped << c;
                break;
            }
            default:
                if (c < 0x20)
                {
                    escaped << "\\u"
                            << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                }
                else
                {
                    escaped << c;
                }
                break;
        }
    }

    return escaped.str();
}