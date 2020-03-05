#pragma once

#include <time.h>
#include <vector>

#include "util.hpp"

struct PageRevision {
    static std::vector<PageRevision> read() {
        std::vector<PageRevision> result;
        std::vector<char> data = readToMemory("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\page_revisions");
        char* bytes = data.data();
        char* end = bytes + data.size();

        char* start = bytes;
        while (bytes != end) {
            PageRevision page_revision;
            int length;

            length = strlen(bytes) + 1;
            page_revision.pageTitle = new char[length];
            memcpy(page_revision.pageTitle, bytes, length);
            bytes += length;

            page_revision.pageId = *((int*) bytes);
            bytes += sizeof(int);

            length = strlen(bytes) + 1;
            page_revision.pageRestrictions = new char[length];
            memcpy(page_revision.pageRestrictions, bytes, length);
            bytes += length;

            page_revision.revisionId = *((int*) bytes);
            bytes += sizeof(int);

            page_revision.revisionTimestamp = *((long long*) bytes);
            bytes += sizeof(long long);

            page_revision.revisionContributorIndex = *((int*) bytes);
            bytes += sizeof(int);

            page_revision.revisionMinor = *((char*) bytes);
            bytes += 1;

            length = strlen(bytes) + 1;
            page_revision.revisionComment = new char[length];
            memcpy(page_revision.revisionComment, bytes, length);
            bytes += length;

            length = strlen(bytes) + 1;
            page_revision.revisionText = new char[length];
            memcpy(page_revision.revisionText, bytes, length);
            bytes += length;

            result.push_back(page_revision);
        }

        return result;
    }

    char* pageTitle;
    int pageId;
    char* pageRestrictions;
    int revisionId;
    time_t revisionTimestamp;
    int revisionContributorIndex;
    bool revisionMinor;
    char* revisionComment;
    char* revisionText;
};
