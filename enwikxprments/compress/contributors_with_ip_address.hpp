#pragma once

#include "contributor.hpp"

union IP {
	unsigned address;
	unsigned char components[4];
};

class ContributorWithIpAddress: public Contributor {
public:
	ContributorWithIpAddress(IP ip): ip(ip) {
	}

	bool operator==(const ContributorWithIpAddress& other) const {
		return this->ip.address == other.ip.address;
	}

	bool operator<(const ContributorWithIpAddress& other) const {
		return this->ip.address < other.ip.address;
	}

private:
	IP ip;
};
