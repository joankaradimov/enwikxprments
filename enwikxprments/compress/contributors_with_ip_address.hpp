#pragma once

#include <functional>
#include <stdio.h>

union IP {
	unsigned address;
	unsigned char components[4];
};

class ContributorWithIpAddress {
public:
	ContributorWithIpAddress(IP ip): ip(ip) {
	}

	bool operator==(const ContributorWithIpAddress& other) const {
		return this->ip.address == other.ip.address;
	}

private:
	IP ip;

	friend class std::hash<ContributorWithIpAddress>;
};

namespace std {
    template <> struct hash<ContributorWithIpAddress> {
        std::size_t operator()(const ContributorWithIpAddress& c) const {
			return std::hash<unsigned>{}(c.ip.address);
        }
    };
}
