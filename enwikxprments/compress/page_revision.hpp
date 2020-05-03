#pragma once

#include <algorithm>
#include <fstream>
#include <set>
#include <vector>

#include "xml/parser"

#include "contributor.hpp"
#include "contributors_with_ip_address.hpp"
#include "contributors_with_ip_string.hpp"
#include "contributors_with_username.hpp"
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
        add(*page_revistion.contributor);
    }

    PageRevision& back() {
        return page_revisions.back();
    }

    void write_binary() const {
        std::vector<ContributorWithUsername> with_username(this->with_username.begin(), this->with_username.end());
        std::vector<ContributorWithIpAddress> with_ip_address(this->with_ip_address.begin(), this->with_ip_address.end());
        std::vector<ContributorWithIpString> with_ip_string(this->with_ip_string.begin(), this->with_ip_string.end());

        std::ofstream username_id_output("out/contributors_with_username_id", std::ios::binary);
        std::ofstream username_username_output("out/contributors_with_username_username", std::ios::binary);
        std::ofstream ip_address_output("out/contributors_with_ip_address", std::ios::binary);
        std::ofstream ip_string_output("out/contributors_with_ip_string", std::ios::binary);

        for (auto contributor : with_username) {
            username_id_output.write((char*)&contributor.id, sizeof(contributor.id));
            username_username_output.write(contributor.username.c_str(), contributor.username.length() + 1);
        }

        for (auto contributor : with_ip_address) {
            ip_address_output.write((char*)&contributor.ip, sizeof(contributor.ip));
        }

        for (auto contributor : with_ip_string) {
            ip_string_output.write(contributor.address.c_str(), contributor.address.length() + 1);
        }

        std::ofstream page_revisions("out/page_revisions", std::ios::binary);
        std::ofstream page_revisions_title("out/page_revisions_title", std::ios::binary);
        std::ofstream page_revisions_comment("out/page_revisions_comment", std::ios::binary);
        std::ofstream page_revisions_text("out/page_revisions_text", std::ios::binary);

        for (auto page_revision : this->page_revisions) {
            page_revisions_title.write(page_revision.page_title.c_str(), page_revision.page_title.length() + 1);
            page_revisions_comment.write(page_revision.revision_comment.c_str(), page_revision.revision_comment.length() + 1);
            page_revisions_text.write(page_revision.revision_text.c_str(), page_revision.revision_text.length() + 1);

            page_revisions.write((char*)&page_revision.page_id, sizeof(page_revision.page_id));
            page_revisions.write((char*)&page_revision.page_restrictions, sizeof(page_revision.page_restrictions));
            page_revisions.write((char*)&page_revision.revision_id, sizeof(page_revision.revision_id));
            page_revisions.write((char*)&page_revision.revision_timestamp, sizeof(page_revision.revision_timestamp));

            const Contributor& contributor = *page_revision.contributor;
            size_t contributor_index;
            if (auto c = dynamic_cast<const ContributorWithUsername*>(&contributor)) {
                contributor_index = std::distance(std::lower_bound(with_username.begin(), with_username.end(), *c), with_username.begin());
            }
            else if (auto c = dynamic_cast<const ContributorWithIpAddress*>(&contributor)) {
                contributor_index = with_username.size() + std::distance(std::lower_bound(with_ip_address.begin(), with_ip_address.end(), *c), with_ip_address.begin());
            }
            else if (auto c = dynamic_cast<const ContributorWithIpString*>(&contributor)) {
                contributor_index = with_username.size() + with_ip_address.size() + std::distance(std::lower_bound(with_ip_string.begin(), with_ip_string.end(), *c), with_ip_string.begin());
            }
            page_revisions.write((char*)&contributor_index, sizeof(contributor_index));
        }
    }

    void read_xml(const char* filename) {
        const char* ns = "http://www.mediawiki.org/xml/export-0.3/";

        std::ifstream input(filename);
        PageRevision page_revision;

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
                                page_revision.contributor.reset(new ContributorWithUsername(contributor));
                            }
                            else if (enwik_parser.name() == "ip") {
                                std::string text_element = enwik_parser.element();
                                IP ip;

                                if (sscanf_s(text_element.c_str(), "%hhu.%hhu.%hhu.%hhu", ip.components, ip.components + 1, ip.components + 2, ip.components + 3) == 4) {
                                    ContributorWithIpAddress contributor(ip);
                                    page_revision.contributor.reset(new ContributorWithIpAddress(contributor));
                                }
                                else {
                                    ContributorWithIpString contributor(text_element);
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
    }

private:
    void add(const ContributorWithUsername& contributor) {
        with_username.emplace(contributor);
    }

    void add(const ContributorWithIpAddress& contributor) {
        with_ip_address.emplace(contributor);
    }

    void add(const ContributorWithIpString& contributor) {
        with_ip_string.emplace(contributor);
    }

    void add(const Contributor& contributor) {
        if (auto c = dynamic_cast<const ContributorWithUsername*>(&contributor)) {
            add(*c);
        }
        else if (auto c = dynamic_cast<const ContributorWithIpAddress*>(&contributor)) {
            add(*c);
        }
        else if (auto c = dynamic_cast<const ContributorWithIpString*>(&contributor)) {
            add(*c);
        }
    }

    std::vector<PageRevision> page_revisions;
    std::set<ContributorWithUsername> with_username;
    std::set<ContributorWithIpAddress> with_ip_address;
    std::set<ContributorWithIpString> with_ip_string;
};
