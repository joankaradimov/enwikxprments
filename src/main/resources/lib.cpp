#include "contributor.hpp"
#include "revision.hpp"
#include "page.hpp"
#include "xmlwriter.hpp"

#include "contributors.hpp"
#include "revisions.hpp"
#include "pages.hpp"

extern Page pages[];

int main() {
    XmlWriter writer(stdout);

    writer.openTag("mediawiki");

    writer.openTag("siteinfo");
    writer.writeTag("sitename", "Wikipedia");
    writer.writeTag("base", "http://en.wikipedia.org/wiki/Main_Page");
    writer.writeTag("generator", "MediaWiki 1.6alpha");
    writer.writeTag("case", "first-letter");
    writer.openTag("namespaces");
    writer.writeTag("namespace", "Media");
    writer.writeTag("namespace", "Special");
    writer.writeTag("namespace");
    writer.writeTag("namespace", "Talk");
    writer.writeTag("namespace", "User");
    writer.writeTag("namespace", "User talk");
    writer.writeTag("namespace", "Wikipedia");
    writer.writeTag("namespace", "Wikipedia talk");
    writer.writeTag("namespace", "Image");
    writer.writeTag("namespace", "Image Talk");
    writer.writeTag("namespace", "Template");
    writer.writeTag("namespace", "Template Talk");
    writer.writeTag("namespace", "Help");
    writer.writeTag("namespace", "Help Talk");
    writer.writeTag("namespace", "Category");
    writer.writeTag("namespace", "Category Talk");
    writer.writeTag("namespace", "Portal");
    writer.writeTag("namespace", "Portal Talk");
    writer.closeTag();
    writer.closeTag();

    for (Page& page : pages) {
        page.write(writer);
    }
    writer.closeTag();
}
