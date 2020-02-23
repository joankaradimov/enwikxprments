#pragma once

#include "xmlwriter.hpp"

class Contributor {
public:
    Contributor(int id, const char* username): id(id), username(username) {
    }

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

private:

    const char* username;
    const int id;
};
