#include <iostream>

#include "page_revision.hpp"

int main(int argc, char** argv)
try {
    for (int arg_index = 1; arg_index < argc; ++arg_index) {
        std::string arg = argv[arg_index];

        if (arg == "--compress") {
            ++arg_index;
            char* path = argv[arg_index];

            PageRevisions page_revisions;

            page_revisions.parse_xml(path);
            page_revisions.dump();
        }
        else {
            // TODO: print help;
            return 1;
        }
    }
}
catch (xml::parsing & error) {
    std::cout
        << "Parsing error at position (" << error.line() << "," << error.column()
        << "); message: " << error.description() << std::endl;
}
