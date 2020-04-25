#pragma once

#include <ctime>
#include <string>

#include "xml/parser"

struct IsoDateTime {
    IsoDateTime(time_t time) : time(time) {}
    IsoDateTime(tm time) : IsoDateTime(mktime(&time)) {}

    operator time_t() const {
        return time;
    }

private:
    time_t time;
};

namespace xml
{
    template <> struct value_traits<IsoDateTime>
    {
        static IsoDateTime parse(std::string s, const parser& p)
        {
            tm time;

            sscanf_s(s.c_str(), "%4d-%2d-%2dT%2d:%2d:%2dZ",
                &time.tm_year, &time.tm_mon, &time.tm_mday,
                &time.tm_hour, &time.tm_min, &time.tm_sec);

            time.tm_year -= 1900;
            time.tm_mon -= 1;

            return time;
        }

        static std::string serialize(IsoDateTime x, const serializer&)
        {
            return ""; // TODO
        }
    };
}
