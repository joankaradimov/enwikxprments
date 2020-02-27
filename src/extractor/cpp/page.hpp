#pragma once

#include "revision.hpp"
#include "xmlwriter.hpp"

class Page {
public:
    Page(const char* title, int id, const char* restrictions, const Revision& revision): title(title), id(id), restrictions(restrictions), revision(revision) {
    }

    void write(XmlWriter xmlWriter) const {
        xmlWriter.openTag("page");
        xmlWriter.writeTag("title", title);
        xmlWriter.writeTag("id", id);
        if (restrictions) {
            xmlWriter.writeTag("restrictions", restrictions);
        }
        revision.write(xmlWriter);
        xmlWriter.closeTag();
    }

private:

    const char* title;
    const int id;
    const char* restrictions;
    const Revision& revision;
};
