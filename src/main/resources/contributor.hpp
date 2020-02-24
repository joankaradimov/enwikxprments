#pragma once

#include "xmlwriter.hpp"

enum ContributorType: char {
    USER,
    IP,
    // TODO: implement IP range
    CONVERSION_SCRIPT,
};

struct Contributor {
    void write(XmlWriter xmlWriter) const {
        xmlWriter.openTag("contributor");
        if (contributorType == USER) {
            if (username) {
                xmlWriter.writeTag("username", username);
            }
            if (id) {
                xmlWriter.writeTag("id", id);
            }
        } else if (contributorType == IP) {
            xmlWriter.writeTag("ip", username);
        } else if (contributorType == CONVERSION_SCRIPT) {
            xmlWriter.writeTag("ip", "Conversion script");
        }
        xmlWriter.closeTag();
    }

    const int id;
    const ContributorType contributorType;
    const char username[];
};
