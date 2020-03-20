#pragma once

#include <time.h>
#include <vector>

#include "binary_reader.hpp"

enum Restrictions {
    NONE,
    EDIT_SYSOP_MOVE_SYSOP,
    MOVE_SYSOP_EDIT_SYSOP,
    MOVE_EDIT,
    MOVE_SYSOP,
    MOVE_AUTOCONFIRMED,
    EDIT_AUTOCONFIRMED_MOVE_SYSOP,
    EDIT_AUTOCONFIRMED_MOVE_AUTOCONFIRMED,
    SYSOP,
};

struct PageRevision {
    static std::vector<PageRevision> read() {
        std::vector<PageRevision> result;
        BinaryReader reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\page_revisions");
        BinaryReader title_reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\page_revisions_title");
        BinaryReader comment_reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\page_revisions_comment");
        BinaryReader text_reader("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\page_revisions_text");

        while (reader.has_more()) {
            PageRevision page_revision;

            page_revision.pageTitle = title_reader.read_string();
            page_revision.pageId = reader.read<int>();
            page_revision.pageRestrictions = reader.read<char>();
            page_revision.revisionId = reader.read<int>();
            page_revision.revisionTimestamp = reader.read<time_t>();
            page_revision.revisionContributorIndex = reader.read<int>();
            page_revision.revisionMinor = reader.read<char>();;
            page_revision.revisionComment = comment_reader.read_string();
            page_revision.revisionText = text_reader.read_string();

            result.push_back(page_revision);
        }

        return result;
    }

    const char* getPageRestrictions() const {
        switch (pageRestrictions) {
            case Restrictions::NONE: return "";
            case Restrictions::EDIT_SYSOP_MOVE_SYSOP: return "edit=sysop:move=sysop";
            case Restrictions::MOVE_SYSOP_EDIT_SYSOP: return "move=sysop:edit=sysop";
            case Restrictions::MOVE_EDIT: return "move=:edit=";
            case Restrictions::MOVE_SYSOP: return "move=sysop";
            case Restrictions::MOVE_AUTOCONFIRMED: return "move=autoconfirmed";
            case Restrictions::EDIT_AUTOCONFIRMED_MOVE_SYSOP: return "edit=autoconfirmed:move=sysop";
            case Restrictions::EDIT_AUTOCONFIRMED_MOVE_AUTOCONFIRMED: return "edit=autoconfirmed:move=autoconfirmed";
            case Restrictions::SYSOP: return "sysop";
        }
    }

    char* pageTitle;
    int pageId;
    char pageRestrictions;
    int revisionId;
    time_t revisionTimestamp;
    int revisionContributorIndex;
    bool revisionMinor;
    char* revisionComment;
    char* revisionText;
};
