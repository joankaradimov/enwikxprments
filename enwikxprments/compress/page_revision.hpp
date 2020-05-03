#pragma once

#include <fstream>
#include <set>
#include <vector>

#include "xml/parser"

#include "contributors.hpp"
#include "iso_date_time.hpp"
#include "restrictions.hpp"

struct PageRevision {
    std::string page_title;
    int page_id;
    Restrictions page_restrictions;
    int revision_id;
    time_t revision_timestamp;
    std::shared_ptr<Contributor> contributor;
    bool revision_minor;
    std::string revision_comment;
    std::string revision_text;
};

class PageRevisions {
public:
    void emplace_back(PageRevision& page_revistion) {
        page_revisions.emplace_back(page_revistion);
    }

    PageRevision& back() {
        return page_revisions.back();
    }

    void read_binary() {
        std::vector<ContributorWithUsername> with_username;
        std::vector<ContributorWithIpAddress> with_ip_address;
        std::vector<ContributorWithIpString> with_ip_string;

        std::ifstream username_id_input("out/contributors_with_username_id", std::ios::binary);
        std::ifstream username_username_input("out/contributors_with_username_username", std::ios::binary);
        while (username_id_input.eof() == false) {
            ContributorWithUsername contributor;
            username_id_input.read((char*)&contributor.id, sizeof(contributor.id));
            username_username_input >> contributor.username;
            with_username.push_back(contributor);
        }
        contributors.swap(with_username);

        std::ifstream ip_address_output("out/contributors_with_ip_address", std::ios::binary);
        while (ip_address_output.eof() == false) {
            ContributorWithIpAddress contributor;
            ip_address_output.read((char*)&contributor.ip, sizeof(contributor.ip));
            with_ip_address.push_back(contributor);
        }
        contributors.swap(with_ip_address);

        std::ifstream ip_string_output("out/contributors_with_ip_string", std::ios::binary);
        while (ip_string_output.eof() == false) {
            ContributorWithIpString contributor;
            ip_string_output >> contributor.address;
            with_ip_string.push_back(contributor);
        }
        contributors.swap(with_ip_string);

        std::ifstream page_revisions_input("out/page_revisions", std::ios::binary);
        std::ifstream page_revisions_title_input("out/page_revisions_title", std::ios::binary);
        std::ifstream page_revisions_comment_input("out/page_revisions_comment", std::ios::binary);
        std::ifstream page_revisions_text_input("out/page_revisions_text", std::ios::binary);

        page_revisions.clear();
        while (page_revisions_input.eof() == false) {
            PageRevision page_revision;
            page_revisions_title_input >> page_revision.page_title;
            page_revisions_comment_input >> page_revision.revision_comment;
            page_revisions_text_input >> page_revision.revision_text;

            page_revisions_input.read((char*)&page_revision.page_id, sizeof(page_revision.page_id));
            page_revisions_input.read((char*)&page_revision.page_restrictions, sizeof(page_revision.page_restrictions));
            page_revisions_input.read((char*)&page_revision.revision_id, sizeof(page_revision.revision_id));
            page_revisions_input.read((char*)&page_revision.revision_timestamp, sizeof(page_revision.revision_timestamp));

            size_t contributor_index;
            page_revisions_input.read((char*)&contributor_index, sizeof(contributor_index));
            page_revision.contributor = contributors.get(contributor_index);

            page_revisions.push_back(page_revision);
        }
    }

    void write_binary() const {
        std::ofstream username_id_output("out/contributors_with_username_id", std::ios::binary);
        std::ofstream username_username_output("out/contributors_with_username_username", std::ios::binary);
        std::ofstream ip_address_output("out/contributors_with_ip_address", std::ios::binary);
        std::ofstream ip_string_output("out/contributors_with_ip_string", std::ios::binary);

        for (auto contributor : contributors.with_username) {
            username_id_output.write((char*)&contributor.id, sizeof(contributor.id));
            username_username_output.write(contributor.username.c_str(), contributor.username.length() + 1);
        }

        for (auto contributor : contributors.with_ip_address) {
            ip_address_output.write((char*)&contributor.ip, sizeof(contributor.ip));
        }

        for (auto contributor : contributors.with_ip_string) {
            ip_string_output.write(contributor.address.c_str(), contributor.address.length() + 1);
        }

        std::ofstream page_revisions_output("out/page_revisions", std::ios::binary);
        std::ofstream page_revisions_title_output("out/page_revisions_title", std::ios::binary);
        std::ofstream page_revisions_comment_output("out/page_revisions_comment", std::ios::binary);
        std::ofstream page_revisions_text_output("out/page_revisions_text", std::ios::binary);

        for (auto page_revision : this->page_revisions) {
            page_revisions_title_output.write(page_revision.page_title.c_str(), page_revision.page_title.length() + 1);
            page_revisions_comment_output.write(page_revision.revision_comment.c_str(), page_revision.revision_comment.length() + 1);
            page_revisions_text_output.write(page_revision.revision_text.c_str(), page_revision.revision_text.length() + 1);

            page_revisions_output.write((char*)&page_revision.page_id, sizeof(page_revision.page_id));
            page_revisions_output.write((char*)&page_revision.page_restrictions, sizeof(page_revision.page_restrictions));
            page_revisions_output.write((char*)&page_revision.revision_id, sizeof(page_revision.revision_id));
            page_revisions_output.write((char*)&page_revision.revision_timestamp, sizeof(page_revision.revision_timestamp));

            const Contributor& contributor = *page_revision.contributor;
            size_t contributor_index = contributors.get_index(contributor);
            page_revisions_output.write((char*)&contributor_index, sizeof(contributor_index));
        }
    }

    void read_xml(const char* filename) {
        const char* ns = "http://www.mediawiki.org/xml/export-0.3/";

        std::ifstream input(filename);

        if (!input.is_open()) {
            auto message = (std::string) "Could not open '" + filename + "'";
            throw std::invalid_argument(message);
        }

        PageRevision page_revision;
        std::set<ContributorWithIpAddress> with_ip_address;
        std::set<ContributorWithIpString> with_ip_string;
        std::set<ContributorWithUsername> with_username;

        try {
            xml::parser enwik_parser(input, filename);

            enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "mediawiki", xml::content::complex);
            enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "siteinfo", xml::content::complex);

            for (auto value_type : enwik_parser) {
                auto x = enwik_parser.name();
                auto y = value_type;
                if (enwik_parser.name() == "namespace" && value_type == xml::parser::event_type::start_element) {
                    enwik_parser.attribute("key");
                }
                if (enwik_parser.name() == "siteinfo" && value_type == xml::parser::event_type::end_element) {
                    break;
                }
            }

            do
            {
                page_revision = PageRevision();

                enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "page", xml::content::value::complex);

                {
                    enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "title", xml::content::value::simple);
                    enwik_parser.next_expect(xml::parser::event_type::characters);
                    page_revision.page_title = enwik_parser.value();
                    enwik_parser.next_expect(xml::parser::event_type::end_element);

                    enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "id", xml::content::value::simple);
                    enwik_parser.next_expect(xml::parser::event_type::characters);
                    page_revision.page_id = enwik_parser.value<int>();
                    enwik_parser.next_expect(xml::parser::event_type::end_element);

                    enwik_parser.next_expect(xml::parser::event_type::start_element);
                    if (enwik_parser.name() == "restrictions") {
                        page_revision.page_restrictions = enwik_parser.element<Restrictions>();
                        enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "revision", xml::content::value::complex);
                    }
                    else if (enwik_parser.name() == "revision") {
                        enwik_parser.content(xml::content::value::complex);
                    }

                    {
                        enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "id", xml::content::value::simple);
                        enwik_parser.next_expect(xml::parser::event_type::characters);
                        page_revision.revision_id = enwik_parser.value<int>();
                        enwik_parser.next_expect(xml::parser::event_type::end_element);

                        enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "timestamp", xml::content::value::simple);
                        enwik_parser.next_expect(xml::parser::event_type::characters);
                        page_revision.revision_timestamp = enwik_parser.value<IsoDateTime>();
                        enwik_parser.next_expect(xml::parser::event_type::end_element);

                        enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "contributor", xml::content::value::complex);
                        {
                            enwik_parser.next_expect(xml::parser::event_type::start_element);

                            if (enwik_parser.name() == "username") {
                                std::string username = enwik_parser.element();

                                enwik_parser.next_expect(xml::parser::event_type::start_element, ns, "id", xml::content::value::simple);
                                enwik_parser.next_expect(xml::parser::event_type::characters);
                                auto id = enwik_parser.value<int>();
                                enwik_parser.next_expect(xml::parser::event_type::end_element);

                                ContributorWithUsername contributor(id, username);
                                with_username.emplace(contributor);
                                page_revision.contributor.reset(new ContributorWithUsername(contributor));
                            }
                            else if (enwik_parser.name() == "ip") {
                                std::string text_element = enwik_parser.element();
                                IP ip;

                                if (sscanf_s(text_element.c_str(), "%hhu.%hhu.%hhu.%hhu", ip.components, ip.components + 1, ip.components + 2, ip.components + 3) == 4) {
                                    ContributorWithIpAddress contributor(ip);
                                    with_ip_address.emplace(contributor);
                                    page_revision.contributor.reset(new ContributorWithIpAddress(contributor));
                                }
                                else {
                                    ContributorWithIpString contributor(text_element);
                                    with_ip_string.emplace(contributor);
                                    page_revision.contributor.reset(new ContributorWithIpString(contributor));
                                }
                            }
                        }
                        enwik_parser.next_expect(xml::parser::event_type::end_element);

                        enwik_parser.next_expect(xml::parser::event_type::start_element);

                        if (enwik_parser.name() == "minor") {
                            enwik_parser.next_expect(xml::parser::event_type::end_element);
                            page_revision.revision_minor = true;
                            enwik_parser.next_expect(xml::parser::event_type::start_element);
                        }

                        if (enwik_parser.name() == "comment") {
                            enwik_parser.next_expect(xml::parser::event_type::characters);
                            page_revision.revision_comment = enwik_parser.element();
                            enwik_parser.next_expect(xml::parser::event_type::start_element);
                        }

                        if (enwik_parser.name() == "text") {
                            for (const auto& attribute_name : enwik_parser.attribute_map()); // Consume attributes
                            page_revision.revision_text = enwik_parser.element();
                        }
                    }
                    enwik_parser.next_expect(xml::parser::event_type::end_element);
                }

                page_revisions.emplace_back(page_revision);

                enwik_parser.next_expect(xml::parser::event_type::end_element);
            } while (enwik_parser.peek() == xml::parser::event_type::start_element);
        }
        catch (xml::parsing& error) {
            if (input.tellg() != EOF) {
                throw;
            }
            page_revisions.emplace_back(page_revision);
        }

        std::vector<ContributorWithIpAddress> with_ip_address_vector(with_ip_address.begin(), with_ip_address.end());
        contributors.swap(with_ip_address_vector);
        std::vector<ContributorWithIpString> with_ip_string_vector(with_ip_string.begin(), with_ip_string.end());
        contributors.swap(with_ip_string_vector);
        std::vector<ContributorWithUsername> with_username_vector(with_username.begin(), with_username.end());
        contributors.swap(with_username_vector);
    }

    void write_xml(const char* filepath) {
        throw std::exception("Not implemented"); // TODO: implement
    }

private:
    std::vector<PageRevision> page_revisions;
    Contributors contributors;
};
