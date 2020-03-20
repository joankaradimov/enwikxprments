#pragma once

#include <string.h>

#include "binary_reader.hpp"

enum ContributorType: char {
    IP = 0,
    USER = 1,
};

struct ContributorWithIp {
    static std::vector<ContributorWithIp> read() {
        std::vector<ContributorWithIp> result;
        BinaryReader reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\contributors_with_ip");

        while (reader.has_more()) {
            ContributorWithIp contributor;

            contributor.ip = reader.read_string();

            result.push_back(contributor);
        }

        return result;
    }

    char* ip;
};

struct ContributorWithUsername {
    static std::vector<ContributorWithUsername> read() {
        std::vector<ContributorWithUsername> result;
        BinaryReader id_reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\contributors_with_username_id");
        BinaryReader username_reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\contributors_with_username_username");

        while (id_reader.has_more()) {
            ContributorWithUsername contributor;

            contributor.id = id_reader.read<int>();
            contributor.username = username_reader.read_string();

            result.push_back(contributor);
        }

        return result;
    }
    int id;
    char* username;
};
