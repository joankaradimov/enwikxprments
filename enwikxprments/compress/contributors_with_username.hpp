#pragma once

#include <string>

#include "contributor.hpp"

struct ContributorWithUsername : public Contributor {
	ContributorWithUsername() {}

	ContributorWithUsername(int id, const std::string& username) : id(id), username(username) {
	}

	bool operator==(const ContributorWithUsername& other) const {
		return this->id == other.id && this->username == other.username;
	}

	bool operator<(const ContributorWithUsername& other) const {
		return this->id != other.id ? this->id < other.id : this->username < other.username;
	}

	int id;
	std::string username;
};
