#pragma once

#include <string.h>

#include "binary_reader.hpp"

enum ContributorType: unsigned char {
    IP_ADDRESS = 0,
    IP_RANGE = 1,
    IP_STRING = 2,
    USERNAME = 3,
};

struct ContributorWithIpAddress {
    static std::vector<ContributorWithIpAddress> read() {
        std::vector<ContributorWithIpAddress> result;
        BinaryReader reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\contributors_with_ip_address");

        while (reader.has_more()) {
            ContributorWithIpAddress contributor;

            contributor.ip_bytes[0] = reader.read<unsigned char>();
            contributor.ip_bytes[1] = reader.read<unsigned char>();
            contributor.ip_bytes[2] = reader.read<unsigned char>();
            contributor.ip_bytes[3] = reader.read<unsigned char>();

            result.push_back(contributor);
        }

        return result;
    }

    unsigned char ip_bytes[4];
};

struct ContributorWithIpRange {
    static std::vector<ContributorWithIpRange> read() {
        std::vector<ContributorWithIpRange> result;
        BinaryReader reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\contributors_with_ip_range");

        while (reader.has_more()) {
            ContributorWithIpRange contributor;

            contributor.ip_bytes[0] = reader.read<unsigned char>();
            contributor.ip_bytes[1] = reader.read<unsigned char>();
            contributor.ip_bytes[2] = reader.read<unsigned char>();

            result.push_back(contributor);
        }

        return result;
    }

    unsigned char ip_bytes[3];
};

struct ContributorWithIpString {
    static std::vector<ContributorWithIpString> read() {
        std::vector<ContributorWithIpString> result;
        BinaryReader reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\contributors_with_ip_string");

        while (reader.has_more()) {
            ContributorWithIpString contributor;

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
