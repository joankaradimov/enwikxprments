#pragma once

#include <string>

#include "xml/parser"

enum class Restrictions {
    NONE,
    EDIT_SYSOP_MOVE_SYSOP,
    MOVE_SYSOP_EDIT_SYSOP,
    MOVE_EDIT,
    MOVE_SYSOP,
    MOVE_AUTOCONFIRMED,
    EDIT_AUTOCONFIRMED_MOVE_SYSOP,
    EDIT_AUTOCONFIRMED_MOVE_AUTOCONFIRMED,
    SYSOP,
};

namespace xml
{
    template <> struct value_traits<Restrictions>
    {
        static Restrictions parse(std::string s, const parser& p)
        {
            if (s == "edit=sysop:move=sysop") return Restrictions::EDIT_SYSOP_MOVE_SYSOP;
            if (s == "move=sysop:edit=sysop") return Restrictions::MOVE_SYSOP_EDIT_SYSOP;
            if (s == "move=:edit=") return Restrictions::MOVE_EDIT;
            if (s == "move=sysop") return Restrictions::MOVE_SYSOP;
            if (s == "move=autoconfirmed") return Restrictions::MOVE_AUTOCONFIRMED;
            if (s == "edit=autoconfirmed:move=sysop") return Restrictions::EDIT_AUTOCONFIRMED_MOVE_SYSOP;
            if (s == "edit=autoconfirmed:move=autoconfirmed") return Restrictions::EDIT_AUTOCONFIRMED_MOVE_AUTOCONFIRMED;
            if (s == "sysop") return Restrictions::SYSOP;
            throw new std::invalid_argument("Invalid restriction argument");
        }

        static std::string serialize(Restrictions restrictions, const serializer&)
        {
            switch (restrictions) {
            case Restrictions::NONE: return "";
            case Restrictions::EDIT_SYSOP_MOVE_SYSOP: return "edit=sysop:move=sysop";
            case Restrictions::MOVE_SYSOP_EDIT_SYSOP: return "move=sysop:edit=sysop";
            case Restrictions::MOVE_EDIT: return "move=:edit=";
            case Restrictions::MOVE_SYSOP: return "move=sysop";
            case Restrictions::MOVE_AUTOCONFIRMED: return "move=autoconfirmed";
            case Restrictions::EDIT_AUTOCONFIRMED_MOVE_SYSOP: return "edit=autoconfirmed:move=sysop";
            case Restrictions::EDIT_AUTOCONFIRMED_MOVE_AUTOCONFIRMED: return "edit=autoconfirmed:move=autoconfirmed";
            case Restrictions::SYSOP: return "sysop";
            }
        }
    };
}
