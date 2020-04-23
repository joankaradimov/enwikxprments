#pragma once

#include <string>

#include "contributor.hpp"

class ContributorWithIpString : public Contributor {
public:
	ContributorWithIpString(const std::string& address) : address(address) {
	}

	bool operator==(const ContributorWithIpString& other) const {
		return this->address == other.address;
	}

	bool operator<(const ContributorWithIpString& other) const {
		return this->address < other.address;
	}

private:
	std::string address;
};
