package com.github.joankaradimov;

import org.mediawiki.xml.export.PageType;
import org.mediawiki.xml.export.RevisionType;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Iterator;

public class PageRevisions implements Iterable<PageRevisions.PageRevision> {
    public static final class PageRevision {
        public PageRevision(PageType page, RevisionType revision, int contributorIndex) {
            pageTitle = page.getTitle();
            pageId = page.getId().intValue(); // TODO: handle overflow
            pageRestrictions = page.getRestrictions() != null ? page.getRestrictions() : "";

            revisionId = revision.getId().intValue(); // TODO: handle overflow
            revisionTimestamp = revision.getTimestamp().toGregorianCalendar().getTimeInMillis() / 1000;
            revisionContributorIndex = contributorIndex;
            revisionMinor = revision.getMinor() != null;
            revisionComment = revision.getComment() != null ? revision.getComment() : "";
            revisionText = revision.getText().getValue();
        }

        public final String pageTitle;
        public final int pageId;
        public final String pageRestrictions; // TODO: use flags
        public final int revisionId;
        public final long revisionTimestamp;
        public final int revisionContributorIndex;
        public final boolean revisionMinor;
        public final String revisionComment;
        public final String revisionText;
    }

    ArrayList<PageRevision> pageRevisions = new ArrayList<>();
    private static byte[] NULL_TERMINATOR = new byte[] { 0 };

    @Override
    public Iterator<PageRevision> iterator() {
        return pageRevisions.iterator();
    }

    public void add(PageRevision pageRevision) {
        pageRevisions.add(pageRevision);
    }

    public void dump(Path outputDirectory) throws IOException {
        File outputFile = outputDirectory.resolve("page_revisions").toFile();
        File textOutputFile = outputDirectory.resolve("page_revisions_text").toFile();

        try (FileOutputStream output = new FileOutputStream(outputFile);
             FileOutputStream textOutput = new FileOutputStream(textOutputFile)) {
            for (PageRevision pageRevision : pageRevisions) {
                output.write(pageRevision.pageTitle.getBytes(StandardCharsets.UTF_8));
                output.write(NULL_TERMINATOR);
                output.write(ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(pageRevision.pageId).array());
                output.write(pageRevision.pageRestrictions.getBytes(StandardCharsets.UTF_8));
                output.write(NULL_TERMINATOR);
                output.write(ByteBuffer.allocate(Integer.BYTES).order(ByteOrder.LITTLE_ENDIAN).putInt(pageRevision.revisionId).array());
                output.write(ByteBuffer.allocate(Long.BYTES).order(ByteOrder.LITTLE_ENDIAN).putLong(pageRevision.revisionTimestamp).array());
                output.write(ByteBuffer.allocate(Integer.BYTES).order(ByteOrder.LITTLE_ENDIAN).putInt(pageRevision.revisionContributorIndex).array());
                output.write(pageRevision.revisionMinor ? 1 : 0);
                output.write(pageRevision.revisionComment.getBytes(StandardCharsets.UTF_8));
                output.write(NULL_TERMINATOR);

                textOutput.write(pageRevision.revisionText.getBytes(StandardCharsets.UTF_8));
                textOutput.write(NULL_TERMINATOR);
            }
        }
    }
}
