#pragma once

#include <time.h>

#include "contributor.hpp"
#include "xmlwriter.hpp"

// TODO: Get rid of this when no longer using libstdc++
// Cause: https://stackoverflow.com/questions/22240973/major-and-minor-macros-defined-in-sys-sysmacros-h-pulled-in-by-iterator
#pragma push_macro("minor")
#undef minor

class Revision {
public:
    Revision(int id, time_t timestamp, int contributorIndex, bool minor, const char* comment): id(id), timestamp(timestamp), contributorIndex(contributorIndex), minor(minor), comment(comment) {
    }

    void write(XmlWriter xmlWriter) const {
        xmlWriter.openTag("revision");
        xmlWriter.writeTag("id", id);
        xmlWriter.writeTag("timestamp", timestamp);
        xmlWriter.writeContributor(contributorIndex);
        if (minor) {
            xmlWriter.writeTag("minor", "");
        }
        if (comment) {
            xmlWriter.writeTag("comment", comment);
        }

        char filename[260];
        sprintf(filename, "C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data\\revision_text_%d", id);
        std::vector<char> text = readToMemory(filename);
        text.push_back('\0');

        xmlWriter.writeTag("text", text.data(), {XmlAttribute("xml:space", "preserve")});
        xmlWriter.closeTag();
    }

private:

    const int id;
    const time_t timestamp;
    const int contributorIndex;
    const bool minor;
    const char* comment;
};

#pragma pop_macro("minor")
