#pragma once

#include <stdio.h>
#include <time.h>
#include <stack>

class XmlWriter {
public:
    XmlWriter(FILE* output): output(output), indentation(0) {
    }

    void openTag(const char* tagName) {
        writeIndentation();
        writeOpeningTag(tagName);
        writeNewLine();
        indentation += 2;
    }

    void closeTag() {
        indentation -= 2;
        writeIndentation();
        writeClosingTag();
        writeNewLine();
    }

    void writeTag(const char* tagName) {
        writeIndentation();
        fprintf(output, "<%s />", tagName);
        writeNewLine();
    }

    void writeTag(const char* tagName, int number) {
        writeIndentation();
        writeOpeningTag(tagName);
        fprintf(output, "%d", number);
        writeClosingTag();
        writeNewLine();
    }

    void writeTag(const char* tagName, const char* string) {
        writeIndentation();
        writeOpeningTag(tagName);
        fputs(string, output);
        writeClosingTag();
        writeNewLine();
    }

    void writeTag(const char* tagName, time_t time) {
        tm time_breakdown;
        gmtime_r(&time, &time_breakdown);
        writeIndentation();

        writeOpeningTag(tagName);
        fprintf(output, "%d-%02d-%02dT%02d:%02d:%02dZ",
            1900 + time_breakdown.tm_year,
            1 + time_breakdown.tm_mon,
            time_breakdown.tm_mday,
            time_breakdown.tm_hour,
            time_breakdown.tm_min,
            time_breakdown.tm_sec);

        writeClosingTag();
        writeNewLine();
    }
private:
    FILE* output;
    int indentation;
    std::stack<const char*> tags;

    void writeOpeningTag(const char* tagName) {
        tags.push(tagName);
        fprintf(output, "<%s>", tagName);
    }

    void writeClosingTag() {
        fprintf(output, "</%s>", tags.top());
        tags.pop();
    }

    void writeIndentation() {
        fprintf(output, "%*s", indentation, "");
    }

    void writeNewLine() {
        fprintf(output, "\n");
    }
};
