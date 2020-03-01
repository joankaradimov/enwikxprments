#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stack>

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
            printf("%s", string);
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
