#pragma once

#include "xmlwriter.hpp"

struct Contributor {
    void write(XmlWriter xmlWriter) const {
        xmlWriter.openTag("contributor");
        if (username) {
            xmlWriter.writeTag("username", username);
        }
        if (id) {
            xmlWriter.writeTag("id", id);
        }
        xmlWriter.closeTag();
    }

    const int id;
    const char username[];
};
