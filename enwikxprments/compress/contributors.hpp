#pragma once

#include <algorithm>
#include <vector>

#include "contributor.hpp"
#include "contributors_with_ip_address.hpp"
#include "contributors_with_ip_string.hpp"
#include "contributors_with_username.hpp"

struct Contributors {
    void swap(std::vector<ContributorWithIpAddress>& with_ip_address) {
        this->with_ip_address.swap(with_ip_address);
    }

    void swap(std::vector<ContributorWithIpString>& with_ip_string) {
        this->with_ip_string.swap(with_ip_string);
    }

    void swap(std::vector<ContributorWithUsername>& with_username) {
        this->with_username.swap(with_username);
    }

    size_t get_index(const Contributor& contributor) const {
        size_t result;
        if (auto c = dynamic_cast<const ContributorWithUsername*>(&contributor)) {
            return get_index(with_username, *c);
        }
        else if (auto c = dynamic_cast<const ContributorWithIpAddress*>(&contributor)) {
            return with_username.size() + get_index(with_ip_address, *c);
        }
        else if (auto c = dynamic_cast<const ContributorWithIpString*>(&contributor)) {
            return with_username.size() + with_ip_address.size() + get_index(with_ip_string, *c);
        }
        else {
            throw new std::invalid_argument("Unsupported contributor type");
        }
    }

    template<typename T>
    static size_t get_index(const std::vector<T>& contributors, const T& contributor) {
        return std::distance(std::lower_bound(contributors.begin(), contributors.end(), contributor), contributors.begin());
    }

    std::vector<ContributorWithIpAddress> with_ip_address;
    std::vector<ContributorWithIpString> with_ip_string;
    std::vector<ContributorWithUsername> with_username;
};
