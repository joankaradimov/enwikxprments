#pragma once

#include <time.h>
#include <vector>

#include "binary_reader.hpp"

struct PageRevision {
    static std::vector<PageRevision> read() {
        std::vector<PageRevision> result;
        BinaryReader reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\page_revisions");
        BinaryReader text_reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\page_revisions_text");

        while (reader.has_more()) {
            PageRevision page_revision;

            page_revision.pageTitle = reader.read_string();
            page_revision.pageId = reader.read<int>();
            page_revision.pageRestrictions = reader.read_string();
            page_revision.revisionId = reader.read<int>();
            page_revision.revisionTimestamp = reader.read<time_t>();
            page_revision.revisionContributorIndex = reader.read<int>();
            page_revision.revisionMinor = reader.read<char>();;
            page_revision.revisionComment = reader.read_string();
            page_revision.revisionText = text_reader.read_string();

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
