#include <fstream>
#include <iostream>

#include "xml/parser"

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
                    std::string restrictions = enwik_parser.element();
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
                    auto timestamp = enwik_parser.value(); // TODO: parse time
                    enwik_parser.next_expect(xml::parser::end_element);

                    enwik_parser.next_expect(xml::parser::start_element, ns, "contributor", xml::content::complex);
                    {
                        enwik_parser.next_expect(xml::parser::start_element);

                        if (enwik_parser.name() == "ip") {
                            std::string ip = enwik_parser.element();
                        }
                        else if (enwik_parser.name() == "username") {
                            std::string username = enwik_parser.element();

                            enwik_parser.next_expect(xml::parser::start_element, ns, "id", xml::content::simple);
                            enwik_parser.next_expect(xml::parser::characters);
                            auto id = enwik_parser.value<int>();
                            enwik_parser.next_expect(xml::parser::end_element);
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
