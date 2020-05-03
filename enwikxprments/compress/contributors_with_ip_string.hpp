#pragma once

#include <string>

#include "contributor.hpp"

struct ContributorWithIpString : public Contributor {
	ContributorWithIpString() {}

	ContributorWithIpString(const std::string& address) : address(address) {
	}

	bool operator==(const ContributorWithIpString& other) const {
		return this->address == other.address;
	}

	bool operator<(const ContributorWithIpString& other) const {
		return this->address < other.address;
	}

	std::string address;
};
