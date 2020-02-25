#include "contributor.hpp"
#include "revision.hpp"
#include "page.hpp"
#include "xmlwriter.hpp"

#include "contributors.hpp"
#include "revisions.hpp"
#include "pages.hpp"

int main() {
    XmlWriter writer(stdout);

    writer.openTag("mediawiki", {
        XmlAttribute("xmlns", "http://www.mediawiki.org/xml/export-0.3/"),
        XmlAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"),
        XmlAttribute("xsi:schemaLocation", "http://www.mediawiki.org/xml/export-0.3/ http://www.mediawiki.org/xml/export-0.3.xsd"),
        XmlAttribute("version", "0.3"),
        XmlAttribute("xml:lang", "en"),
    });

    writer.openTag("siteinfo");
    writer.writeTag("sitename", "Wikipedia");
    writer.writeTag("base", "http://en.wikipedia.org/wiki/Main_Page");
    writer.writeTag("generator", "MediaWiki 1.6alpha");
    writer.writeTag("case", "first-letter");
    writer.printf("  "); // This is a special case in the enwik dataset
    writer.openTag("namespaces");
    writer.writeTag("namespace", "Media", {XmlAttribute("key", "-2")});
    writer.writeTag("namespace", "Special", {XmlAttribute("key", "-1")});
    writer.writeTag("namespace", "", {XmlAttribute("key", "0")});
    writer.writeTag("namespace", "Talk", {XmlAttribute("key", "1")});
    writer.writeTag("namespace", "User", {XmlAttribute("key", "2")});
    writer.writeTag("namespace", "User talk", {XmlAttribute("key", "3")});
    writer.writeTag("namespace", "Wikipedia", {XmlAttribute("key", "4")});
    writer.writeTag("namespace", "Wikipedia talk", {XmlAttribute("key", "5")});
    writer.writeTag("namespace", "Image", {XmlAttribute("key", "6")});
    writer.writeTag("namespace", "Image talk", {XmlAttribute("key", "7")});
    writer.writeTag("namespace", "MediaWiki", {XmlAttribute("key", "8")});
    writer.writeTag("namespace", "MediaWiki talk", {XmlAttribute("key", "9")});
    writer.writeTag("namespace", "Template", {XmlAttribute("key", "10")});
    writer.writeTag("namespace", "Template talk", {XmlAttribute("key", "11")});
    writer.writeTag("namespace", "Help", {XmlAttribute("key", "12")});
    writer.writeTag("namespace", "Help talk", {XmlAttribute("key", "13")});
    writer.writeTag("namespace", "Category", {XmlAttribute("key", "14")});
    writer.writeTag("namespace", "Category talk", {XmlAttribute("key", "15")});
    writer.writeTag("namespace", "Portal", {XmlAttribute("key", "100")});
    writer.writeTag("namespace", "Portal talk", {XmlAttribute("key", "101")});
    writer.closeTag();
    writer.closeTag();

    for (const Page& page : pages) {
        page.write(writer);
    }
    writer.closeTag();
}
