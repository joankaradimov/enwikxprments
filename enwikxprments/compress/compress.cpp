#include <ctime>
#include <fstream>
#include <iostream>

#include "xml/parser"

#include "contributors_with_ip_address.hpp"
#include "contributors_with_ip_string.hpp"
#include "contributors_with_username.hpp"

struct IsoDateTime {
    IsoDateTime(time_t time): time(time) {}
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

int main()
{
    const char* ns = "http://www.mediawiki.org/xml/export-0.3/";

    const char* input_path = "C:\\Users\\joank\\work\\enwikxprments\\build\\enwik9";
    std::ifstream input(input_path);

    try {
        xml::parser enwik_parser(input, input_path);

        enwik_parser.next_expect(xml::parser::start_element, ns, "mediawiki", xml::content::complex);
        enwik_parser.next_expect(xml::parser::start_element, ns, "siteinfo", xml::content::complex);

        for (auto value_type : enwik_parser) {
            auto x = enwik_parser.name();
            auto y = value_type;
            if (enwik_parser.name() == "namespace" && value_type == xml::parser::start_element) {
                enwik_parser.attribute("key");
            }
            if (enwik_parser.name() == "siteinfo" && value_type == xml::parser::end_element) {
                break;
            }
        }

        do
        {
            enwik_parser.next_expect(xml::parser::start_element, ns, "page", xml::content::complex);

            {
                enwik_parser.next_expect(xml::parser::start_element, ns, "title", xml::content::simple);
                enwik_parser.next_expect(xml::parser::characters);
                auto title = enwik_parser.value();
                enwik_parser.next_expect(xml::parser::end_element);

                enwik_parser.next_expect(xml::parser::start_element, ns, "id", xml::content::simple);
                enwik_parser.next_expect(xml::parser::characters);
                auto id = enwik_parser.value<int>();
                enwik_parser.next_expect(xml::parser::end_element);

                enwik_parser.next_expect(xml::parser::start_element);
                if (enwik_parser.name() == "restrictions") {
                    Restrictions restrictions = enwik_parser.element<Restrictions>();
                    enwik_parser.next_expect(xml::parser::start_element, ns, "revision", xml::content::complex);
                }
                else if (enwik_parser.name() == "revision") {
                    enwik_parser.content(xml::content::complex);
                }

                {
                    enwik_parser.next_expect(xml::parser::start_element, ns, "id", xml::content::simple);
                    enwik_parser.next_expect(xml::parser::characters);
                    auto id = enwik_parser.value<int>();
                    enwik_parser.next_expect(xml::parser::end_element);

                    enwik_parser.next_expect(xml::parser::start_element, ns, "timestamp", xml::content::simple);
                    enwik_parser.next_expect(xml::parser::characters);
                    auto timestamp = enwik_parser.value<IsoDateTime>();
                    enwik_parser.next_expect(xml::parser::end_element);


                    enwik_parser.next_expect(xml::parser::start_element, ns, "contributor", xml::content::complex);
                    {
                        enwik_parser.next_expect(xml::parser::start_element);

                        if (enwik_parser.name() == "username") {
                            std::string username = enwik_parser.element();

                            enwik_parser.next_expect(xml::parser::start_element, ns, "id", xml::content::simple);
                            enwik_parser.next_expect(xml::parser::characters);
                            auto id = enwik_parser.value<int>();
                            enwik_parser.next_expect(xml::parser::end_element);

                            ContributorWithUsername contributor(id, username);
                        }
                        else if (enwik_parser.name() == "ip") {
                            std::string text_element = enwik_parser.element();
                            IP ip;

                            if (sscanf_s(text_element.c_str(), "%hhu.%hhu.%hhu.%hhu", ip.components, ip.components + 1, ip.components + 2, ip.components + 3) == 4) {
                                ContributorWithIpAddress contributor(ip);
                            }
                            else {
                                ContributorWithIpString contributor(text_element);
                            }
                        }
                    }
                    enwik_parser.next_expect(xml::parser::end_element);

                    enwik_parser.next_expect(xml::parser::start_element);

                    if (enwik_parser.name() == "minor") {
                        enwik_parser.next_expect(xml::parser::end_element);
                        enwik_parser.next_expect(xml::parser::start_element);
                    }

                    if (enwik_parser.name() == "comment") {
                        enwik_parser.next_expect(xml::parser::characters);
                        std::string comment = enwik_parser.element();
                        enwik_parser.next_expect(xml::parser::start_element);
                    }

                    if (enwik_parser.name() == "text") {
                        for (const auto& attribute_name : enwik_parser.attribute_map()); // Consume attributes
                        std::string text = enwik_parser.element();
                    }
                }
                enwik_parser.next_expect(xml::parser::end_element);
            }

            enwik_parser.next_expect(xml::parser::end_element);
        } while (enwik_parser.peek() == xml::parser::start_element);
    }
    catch (xml::parsing & error) {
        if (input.tellg() != EOF) {
            std::cout
                << "Parsing error at position (" << error.line() << "," << error.column()
                << "); message: " << error.description() << std::endl;
        }
    }
}
