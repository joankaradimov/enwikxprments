#pragma once

#include <functional>
#include <string>

class ContributorWithIpString {
public:
	ContributorWithIpString(const std::string& address) : address(address) {
	}

	bool operator==(const ContributorWithIpString& other) const {
		return this->address == other.address;
	}

private:
	std::string address;

	friend class std::hash<ContributorWithIpString>;
};

namespace std {
	template <> struct hash<ContributorWithIpString> {
		std::size_t operator()(const ContributorWithIpString& c) const {
			return std::hash<std::string>{}(c.address);
		}
	};
}
