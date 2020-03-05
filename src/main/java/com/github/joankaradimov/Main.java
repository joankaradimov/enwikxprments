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
import java.util.*;

public class Main {
    public static void main(String[] args) {
        try (FileInputStream stream = new FileInputStream("build/enwik9")) {
            ByteArrayInputStream xmlClosingElements = new ByteArrayInputStream("</text></revision></page></mediawiki>".getBytes());
            SequenceInputStream validXmlStream = new SequenceInputStream(stream, xmlClosingElements);

            JAXBContext jaxbContext = JAXBContext.newInstance(org.mediawiki.xml.export.ObjectFactory.class);
            Unmarshaller jaxbUnmarshaller = jaxbContext.createUnmarshaller();
            JAXBElement<MediaWikiType> element = (JAXBElement<MediaWikiType>) jaxbUnmarshaller.unmarshal(validXmlStream);
            MediaWikiType mediaWiki = element.getValue();

            Path outputDirectory = Path.of("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\cpp");
            Path dataOutputDirectory = Path.of("C:\\Users\\joank\\work\\enwikxprments\\src\\extractor\\data");

            ContributorsWithUsername contributorsWithUsername = new ContributorsWithUsername();
            ContributorsWithIp contributorsWithIp = new ContributorsWithIp();

            for (PageType page : mediaWiki.getPage()) {
                List<Object> revisionOrUpload = page.getRevisionOrUpload();

                if (revisionOrUpload.size() != 1) {
                    throw new RuntimeException("Expected exactly one revision or upload");
                }

                if (revisionOrUpload.get(0) instanceof RevisionType) {
                    RevisionType revision = (RevisionType) revisionOrUpload.get(0);
                    ContributorType contributor = revision.getContributor();

                    if (contributor.getId() != null) {
                        // TODO: check for int overflow
                        var c = new ContributorsWithUsername.Contributor(contributor.getId().intValue(), contributor.getUsername());
                        contributorsWithUsername.add(c);
                    } else if (contributor.getIp() != null) {
                        var c = new ContributorsWithIp.Contributor(contributor.getIp());
                        contributorsWithIp.add(c);
                    } else {
                        throw new RuntimeException("Contributor expected to have either an IP or an ID");
                    }
                }
            }

            PageRevisions pageRevisions = new PageRevisions();

            Map<String, Integer> dictionary = new HashMap<>();
            List<List<String>> tokensList = new ArrayList<>();
            int wordCount = 0;

            for (PageType page : mediaWiki.getPage()) {
                List<Object> revisionOrUpload = page.getRevisionOrUpload();

                if (revisionOrUpload.size() == 1 && revisionOrUpload.get(0) instanceof RevisionType) {
                    RevisionType revision = (RevisionType) revisionOrUpload.get(0);
                    ContributorType contributor = revision.getContributor();
                    int index;

                    if (contributor.getId() != null) {
                        // TODO: check for int overflow
                        var c = new ContributorsWithUsername.Contributor(contributor.getId().intValue(), contributor.getUsername());
                        index = contributorsWithUsername.getIndex(c);
                    } else if (contributor.getIp() != null) {
                        var c = new ContributorsWithIp.Contributor(contributor.getIp());
                        index = contributorsWithIp.getIndex(c);
                    } else {
                        throw new RuntimeException("Contributor expected to have either an IP or an ID");
                    }

                    pageRevisions.add(new PageRevisions.PageRevision(page, revision, index));
                } else {
                    throw new RuntimeException("Expected exactly one revision");
                }
            }

            for (PageRevisions.PageRevision pageRevision : pageRevisions) {
                List<String> tokens = tokenize(pageRevision.revisionText);
                tokensList.add(tokens);

                wordCount += tokens.size();
                for (var token : tokens) {
                    int count = dictionary.getOrDefault(token, 0);
                    dictionary.put(token, count + 1);
                }
            }

            contributorsWithUsername.dump(dataOutputDirectory);
            contributorsWithIp.dump(dataOutputDirectory);
            pageRevisions.dump(dataOutputDirectory);

            System.out.print("DICTIONARY SIZE: ");
            System.out.println(dictionary.size());

            System.out.print("WORD COUNT: ");
            System.out.println(wordCount);

            System.out.print("NON-REPEATING WORD COUNT: ");
            System.out.println(dictionary.entrySet().stream().filter(entry -> entry.getValue() == 1).count());
        } catch (IOException | JAXBException e) {
            e.printStackTrace();
        }
    }

    private static enum TokenType {
        ALPHABETIC,
        NUMERIC,
        OTHER,
    }

    private static List<String> tokenize(String text) {
        List<String> result = new ArrayList<>();
        StringBuilder token = new StringBuilder();
        TokenType tokenType= TokenType.OTHER;

        for (int i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            if (Character.isLetter(c)) {
                if (token.length() == 0) {
                    tokenType = TokenType.ALPHABETIC;
                } else if (tokenType != TokenType.ALPHABETIC) {
                    result.add(token.toString());
                    token.setLength(0);
                }
                token.append(c);
            } else if (Character.isDigit(c)) {
                if (token.length() == 0) {
                    tokenType = TokenType.NUMERIC;
                } else if (tokenType != TokenType.NUMERIC) {
                    result.add(token.toString());
                    token.setLength(0);
                }
                token.append(c);
            } else {
                if (token.length() != 0) {
                    result.add(token.toString());
                    token.setLength(0);
                }
                result.add(Character.toString(c));
            }
        }
        if (token.length() != 0) {
            result.add(token.toString());
        }

        return result;
    }

    private static PrintStream createCppPrintStream(Path outputDirectory, String filename) throws IOException {
        return new PrintStream(outputDirectory.resolve(filename).toFile(), StandardCharsets.UTF_8);
    }
}
