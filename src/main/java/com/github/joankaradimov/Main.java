package com.github.joankaradimov;

import org.mediawiki.xml.export.ContributorType;
import org.mediawiki.xml.export.MediaWikiType;
import org.mediawiki.xml.export.PageType;
import org.mediawiki.xml.export.RevisionType;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;
import java.io.*;
import java.math.BigInteger;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class Main {
    public static void main(String[] args) {
        try (FileInputStream stream = new FileInputStream("build/enwik8")) {
            ByteArrayInputStream xmlClosingElements = new ByteArrayInputStream("</text></revision></page></mediawiki>".getBytes());
            SequenceInputStream validXmlStream = new SequenceInputStream(stream, xmlClosingElements);

            JAXBContext jaxbContext = JAXBContext.newInstance(org.mediawiki.xml.export.ObjectFactory.class);
            Unmarshaller jaxbUnmarshaller = jaxbContext.createUnmarshaller();
            JAXBElement element = (JAXBElement) jaxbUnmarshaller.unmarshal(validXmlStream);
            MediaWikiType mediaWiki = (MediaWikiType) element.getValue();

            Path outputDirectory = Path.of("C:\\Users\\joank\\work\\enwikxprments\\src\\main\\resources");

            try (PrintStream pagesStream = createCppPrintStream(outputDirectory, "pages.hpp");
                 PrintStream revisionsStream = createCppPrintStream(outputDirectory, "revisions.hpp");
                 PrintStream contributorsStream = createCppPrintStream(outputDirectory, "contributors.hpp")) {

                pagesStream.printf("const Page pages[] = {\n");
                Set<BigInteger> contributorIds = new HashSet<>();
                Set<String> contributorIps = new HashSet<>();

                for (PageType page : mediaWiki.getPage()) {
                    List<Object> revisionOrUpload = page.getRevisionOrUpload();

                    if (revisionOrUpload.size() != 1) {
                        throw new RuntimeException("Expected exactly one revision or upload");
                    }

                    if (revisionOrUpload.get(0) instanceof RevisionType) {
                        RevisionType revision = (RevisionType) revisionOrUpload.get(0);
                        ContributorType contributor = revision.getContributor();

                        if (contributor.getId() != null) {
                            if (!contributorIds.contains(contributor.getId())) {
                                contributorIds.add(contributor.getId());
                                contributorsStream.printf(
                                        "const Contributor contributor_%d = {%d, USER, %s};\n",
                                        contributor.getId(),
                                        contributor.getId(), // properly handle contributors without ID
                                        escapeString(contributor.getUsername()));
                            }
                        } else if (contributor.getIp() != null) {
                            if (!contributorIps.contains(contributor.getIp())) {
                                contributorIps.add(contributor.getIp());
                                contributorsStream.printf(
                                        "const Contributor contributor_%s = {0, IP, %s};\n",
                                        contributor.getIp().replaceAll("(\\.|\\s)", "_"),
                                        escapeString(contributor.getIp()));
                            }
                        } else {
                            throw new RuntimeException("Contributor expected to have either an IP or an ID");
                        }

                        revisionsStream.printf(
                                "const Revision revision_%d(%d, %d, contributor_%s, %s, %s, %s);\n",
                                revision.getId(),
                                revision.getId(),
                                revision.getTimestamp().toGregorianCalendar().getTimeInMillis() / 1000,
                                contributor.getId() != null ? contributor.getId() : contributor.getIp().replaceAll("(\\.|\\s)", "_"),
                                revision.getMinor() != null ? "true" : "false",
                                escapeString(revision.getComment()),
                                escapeString(revision.getText().getValue()));

                        pagesStream.printf(
                                "  Page(%s, %d, %s, revision_%d),\n",
                                escapeString(page.getTitle()),
                                page.getId(),
                                escapeString(page.getRestrictions()),
                                revision.getId());
                    }
                }
                pagesStream.printf("};");
            }
        } catch (IOException | JAXBException e) {
            e.printStackTrace();
        }
    }

    private static PrintStream createCppPrintStream(Path outputDirectory, String filename) throws IOException {
        return new PrintStream(outputDirectory.resolve(filename).toFile(), StandardCharsets.UTF_8);
    }

    private static String escapeString(String string) {
        if (string == null) {
            return "nullptr";
        }

        String escapedString = string
                .replaceAll("\\\\", "\\\\\\\\")
                .replaceAll("&", "&amp;")
                .replaceAll("\"", "&quot;")
                .replaceAll("<", "&lt;")
                .replaceAll(">", "&gt;")
                .replaceAll("\n", "\\\\n\\\\\n");

        return "\"" + escapedString + "\"";
    }
}
