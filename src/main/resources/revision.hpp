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
    Revision(int id, time_t timestamp, Contributor& contributor, bool minor, const char* comment, const char* text): id(id), timestamp(timestamp), contributor(contributor), minor(minor), comment(comment), text(text) {
    }

    void write(XmlWriter xmlWriter) const {
        xmlWriter.openTag("revision");
        xmlWriter.writeTag("id", id);
        xmlWriter.writeTag("timestamp", timestamp);
        contributor.write(xmlWriter);
        if (minor) {
            xmlWriter.writeTag("minor");
        }
        if (comment) {
            xmlWriter.writeTag("comment", comment);
        }
        xmlWriter.writeTag("text", text, {XmlAttribute("xml:space", "preserve")});
        xmlWriter.closeTag();
    }

private:

    const int id;
    const time_t timestamp;
    const Contributor& contributor;
    const bool minor;
    const char* comment;
    const char* text;
};

#pragma pop_macro("minor")
