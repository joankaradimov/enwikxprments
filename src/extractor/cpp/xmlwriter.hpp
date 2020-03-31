#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stack>

#include "contributor.hpp"
#include "page_revision.hpp"

class XmlAttribute {
public:
    XmlAttribute(const char* name, const char* value): name(name), value(value) {
    }

    const char* getName() const {
        return name;
    }

    const char* getValue() const {
        return value;
    }

private:

    const char* name;
    const char* value;
};

class XmlWriter {
public:
    XmlWriter(FILE* output): output(output), indentation(0) {
        contributorsWithIpAddress = ContributorWithIpAddress::read();
        contributorsWithIpRange = ContributorWithIpRange::read();
        contributorsWithIpString = ContributorWithIpString::read();
        contributorsWithUsername = ContributorWithUsername::read();
    }

    void openTag(const char* tagName, std::initializer_list<XmlAttribute> attributes = {}) {
        writeIndentation();
        writeOpeningTag(tagName, attributes);
        writeNewLine();
        indentation += 2;
    }

    void closeTag() {
        indentation -= 2;
        writeIndentation();
        writeClosingTag();
        writeNewLine();
    }

    void writeTag(const char* tagName, int number) {
        writeIndentation();
        writeOpeningTag(tagName);
        printf("%d", number);
        writeClosingTag();
        writeNewLine();
    }

    void writeTag(const char* tagName, const char* string, const std::initializer_list<XmlAttribute>& attributes = {}) {
        writeIndentation();
        if (*string) {
            writeOpeningTag(tagName, attributes);
            writeTextNode(string);
            writeClosingTag();
        } else {
            printf("<%s", tagName);
            writeAttributes(attributes);
            printf(" />", tagName);
        }
        writeNewLine();
    }

    void writeTag(const char* tagName, time_t time) {
        tm time_breakdown;
        gmtime_s(&time_breakdown, &time);
        writeIndentation();

        writeOpeningTag(tagName);
        printf("%d-%02d-%02dT%02d:%02d:%02dZ",
            1900 + time_breakdown.tm_year,
            1 + time_breakdown.tm_mon,
            time_breakdown.tm_mday,
            time_breakdown.tm_hour,
            time_breakdown.tm_min,
            time_breakdown.tm_sec);

        writeClosingTag();
        writeNewLine();
    }

    void writeTextNode(const char* text) {
        while (*text) {
            switch (*text) {
                case '"':
                    fputs("&quot;", output);
                    break;
                case '<':
                    fputs("&lt;", output);
                    break;
                case '>':
                    fputs("&gt;", output);
                    break;
                case '&':
                    fputs("&amp;", output);
                    break;
                default:
                    fputc(*text, output);
                    break;
            }

            ++text;
        }
    }

    void writePage(const PageRevision& pageRevision) {
        openTag("page");
        writeTag("title", pageRevision.pageTitle);
        writeTag("id", pageRevision.pageId);

        if (pageRevision.pageRestrictions) {
            writeTag("restrictions", pageRevision.getPageRestrictions());
        }

        writeRevision(pageRevision);

        closeTag();
    }

    void writeRevision(const PageRevision& pageRevision) {
        openTag("revision");
        writeTag("id", pageRevision.revisionId);
        writeTag("timestamp", pageRevision.revisionTimestamp);
        writeContributor(pageRevision.revisionContributorIndex);
        if (pageRevision.revisionMinor) {
            writeTag("minor", "");
        }
        if (*pageRevision.revisionComment) {
            writeTag("comment", pageRevision.revisionComment);
        }

        writeTag("text", pageRevision.revisionText, {XmlAttribute("xml:space", "preserve")});
        closeTag();
    }

    void writeContributor(int contributorIndex) {
        int type = contributorIndex & 3;
        int index = contributorIndex >> 2;

        openTag("contributor");

        if (type == ContributorType::IP_ADDRESS) {
            writeIndentation();
            writeOpeningTag("ip");
            unsigned char* ip_bytes = contributorsWithIpAddress[index].ip_bytes;
            printf("%d.%d.%d.%d", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
            writeClosingTag();
            writeNewLine();
        }
        else if (type == ContributorType::IP_RANGE) {
            writeIndentation();
            writeOpeningTag("ip");
            unsigned char* ip_bytes = contributorsWithIpRange[index].ip_bytes;
            printf("%d.%d.%d.xxx", ip_bytes[0], ip_bytes[1], ip_bytes[2]);
            writeClosingTag();
            writeNewLine();
        }
        else if (type == ContributorType::IP_STRING) {
            writeTag("ip", contributorsWithIpString[index].ip);
        }
        else { // Username
            ContributorWithUsername contributor = contributorsWithUsername[index];
            if (contributor.username) {
                writeTag("username", contributor.username);
            }
            if (contributor.id) {
                writeTag("id", contributor.id);
            }
        }

        closeTag();
    }

    int printf(const char* format, ...) {
        va_list arglist;
        va_start(arglist, format);
        int result = vfprintf(output, format, arglist);
        va_end(arglist);
        return result;
    }
private:
    FILE* output;
    int indentation;
    std::stack<const char*> tags;
    std::vector<ContributorWithIpAddress> contributorsWithIpAddress;
    std::vector<ContributorWithIpRange> contributorsWithIpRange;
    std::vector<ContributorWithIpString> contributorsWithIpString;
    std::vector<ContributorWithUsername> contributorsWithUsername;

    void writeOpeningTag(const char* tagName, const std::initializer_list<XmlAttribute>& attributes = {}) {
        tags.push(tagName);
        printf("<%s", tagName);
        writeAttributes(attributes);
        printf(">", tagName);
    }

    void writeAttributes(const std::initializer_list<XmlAttribute>& attributes = {}) {
        for (auto& attribute : attributes) {
            printf(" %s=\"%s\"", attribute.getName(), attribute.getValue());
        }
    }

    void writeClosingTag() {
        printf("</%s>", tags.top());
        tags.pop();
    }

    void writeIndentation() {
        printf("%*s", indentation, "");
    }

    void writeNewLine() {
        printf("\n");
    }
};
