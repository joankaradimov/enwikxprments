#pragma once

#include <string.h>

#include "util.hpp"

enum ContributorType: char {
    IP = 0,
    USER = 1,
};

struct ContributorWithIp {
    static std::vector<ContributorWithIp> read() {
        std::vector<ContributorWithIp> result;
        std::vector<char> data = readToMemory("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\contributors_with_ip");
        char* bytes = data.data();
        char* end = bytes + data.size();

        while (bytes != end) {
            ContributorWithIp contributor;
            int length = strlen(bytes) + 1;
            contributor.ip = new char[length];
            memcpy(contributor.ip, bytes, length);
            bytes += length;

            result.push_back(contributor);
        }

        return result;
    }

    char* ip;
};

struct ContributorWithUsername {
    static std::vector<ContributorWithUsername> read() {
        std::vector<ContributorWithUsername> result;
        std::vector<char> data = readToMemory("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\contributors_with_username");
        char* bytes = data.data();
        char* end = bytes + data.size();

        while (bytes != end) {
            ContributorWithUsername contributor;
            contributor.id = *((int*) bytes);
            bytes += sizeof(int);
            int length = strlen(bytes) + 1;
            contributor.username = new char[length];
            memcpy(contributor.username, bytes, length);
            bytes += length;

            result.push_back(contributor);
        }

        return result;
    }
    int id;
    char* username;
};
