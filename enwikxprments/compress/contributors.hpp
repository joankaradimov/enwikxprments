#pragma once

#include <set>

#include "contributor.hpp"
#include "contributors_with_ip_address.hpp"
#include "contributors_with_ip_string.hpp"
#include "contributors_with_username.hpp"

class Contributors {
public:
    void add(const ContributorWithUsername& contributor) {
        with_username.emplace(contributor);
    }

    void add(const ContributorWithIpAddress& contributor) {
        with_ip_address.emplace(contributor);
    }

    void add(const ContributorWithIpString& contributor) {
        with_ip_string.emplace(contributor);
    }

private:
    std::set<ContributorWithUsername> with_username;
    std::set<ContributorWithIpAddress> with_ip_address;
    std::set<ContributorWithIpString> with_ip_string;
};
