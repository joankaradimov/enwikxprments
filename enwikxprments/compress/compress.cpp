#include <iostream>

#include "page_revision.hpp"

int main()
try {
    const char* input_path = "C:\\Users\\joank\\work\\enwikxprments\\build\\enwik9";

    PageRevisions page_revisions;

    page_revisions.parse_xml(input_path);
    page_revisions.dump();
}
catch (xml::parsing & error) {
    std::cout
        << "Parsing error at position (" << error.line() << "," << error.column()
        << "); message: " << error.description() << std::endl;
}
