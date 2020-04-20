#pragma once

#include <string>

class ContributorWithUsername {
public:
	ContributorWithUsername(int id, const std::string& username) : id(id), username(username) {
	}

	bool operator==(const ContributorWithUsername& other) const {
		return this->id == other.id && this->username == other.username;
	}

	bool operator<(const ContributorWithUsername& other) const {
		return this->id != other.id ? this->id < other.id : this->username < other.username;
	}
private:
	int id;
	std::string username;
};
