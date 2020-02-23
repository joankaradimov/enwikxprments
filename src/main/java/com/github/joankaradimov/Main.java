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

            try (PrintStream pagesStream = new PrintStream(outputDirectory.resolve("pages.hpp").toFile(), StandardCharsets.UTF_8);
                 PrintStream revisionsStream = new PrintStream(outputDirectory.resolve("revisions.hpp").toFile(), StandardCharsets.UTF_8);
                 PrintStream contributorsStream = new PrintStream(outputDirectory.resolve("contributors.hpp").toFile(), StandardCharsets.UTF_8)) {

                pagesStream.printf("Page pages[] = {\n");
                Set<BigInteger> contributorIds = new HashSet<>();

                for (PageType page : mediaWiki.getPage()) {
                    List<Object> revisionOrUpload = page.getRevisionOrUpload();

                    if (revisionOrUpload.size() != 1) {
                        throw new RuntimeException("Expected exactly one revision or upload");
                    }

                    if (revisionOrUpload.get(0) instanceof RevisionType) {
                        RevisionType revision = (RevisionType) revisionOrUpload.get(0);
                        ContributorType contributor = revision.getContributor();

                        if (!contributorIds.contains(contributor.getId())) {
                            contributorIds.add(contributor.getId());
                            contributorsStream.printf(
                                    "Contributor contributor_%d(%d, %s);\n",
                                    contributor.getId(),
                                    contributor.getId() != null ? contributor.getId() : -1, // properly handle contributors without ID
                                    escapeString(contributor.getUsername()));
                        }

                        revisionsStream.printf(
                                "Revision revision_%d(%d, %d, contributor_%d, %s, %s, %s);\n",
                                revision.getId(),
                                revision.getId(),
                                revision.getTimestamp().toGregorianCalendar().getTimeInMillis() / 1000,
                                contributor.getId(),
                                revision.getMinor() != null ? "true" : "false",
                                escapeString(revision.getComment()),
                                escapeString(revision.getText().getValue()));

                        pagesStream.printf(
                                "  Page(%s, %d, revision_%d),\n",
                                escapeString(page.getTitle()),
                                page.getId(),
                                revision.getId());
                    }
                }
                pagesStream.printf("};");
            }
        } catch (IOException | JAXBException e) {
            e.printStackTrace();
        }
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
