#pragma once

#include <algorithm>
#include <fstream>
#include <set>
#include <vector>

#include "contributor.hpp"
#include "contributors_with_ip_address.hpp"
#include "contributors_with_ip_string.hpp"
#include "contributors_with_username.hpp"

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

    void dump() const {
        std::vector<ContributorWithUsername> with_username(this->with_username.begin(), this->with_username.end());
        std::vector<ContributorWithIpAddress> with_ip_address(this->with_ip_address.begin(), this->with_ip_address.end());
        std::vector<ContributorWithIpString> with_ip_string(this->with_ip_string.begin(), this->with_ip_string.end());

        std::ofstream username_id_output("out/contributors_with_username_id");
        std::ofstream username_username_output("out/contributors_with_username_username");
        std::ofstream ip_address_output("out/contributors_with_ip_address");
        std::ofstream ip_string_output("out/contributors_with_ip_string");

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

        std::ofstream page_revisions("out/page_revisions");
        std::ofstream page_revisions_title("out/page_revisions_title");
        std::ofstream page_revisions_comment("out/page_revisions_comment");
        std::ofstream page_revisions_text("out/page_revisions_text");

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
