package com.github.joankaradimov;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.Objects;
import java.util.TreeSet;

public class ContributorsWithUsername {
    public static final class Contributor implements Comparable<Contributor> {
        public Contributor(int id, String username) {
            this.id = id;
            this.username = username;
        }

        public final int id;
        public final String username;

        @Override
        public int compareTo(Contributor other) {
            int idComparison = Integer.compare(this.id, other.id);
            if (idComparison == 0) {
                return this.username.compareTo(other.username);
            } else {
                return idComparison;
            }
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Contributor that = (Contributor) o;
            return id == that.id && username.equals(that.username);
        }

        @Override
        public int hashCode() {
            return Objects.hash(id, username);
        }
    }

    public static final int CONTRIBUTOR_TYPE = 1; // TODO: use an enum
    private static byte[] NULL_TERMINATOR = new byte[] { 0 };

    private final TreeSet<Contributor> contributors = new TreeSet<>();

    public void add(Contributor contributor) {
        contributors.add(contributor);
    }

    public int getIndex(Contributor contributor) {
        return (contributors.headSet(contributor).size() << 1) | CONTRIBUTOR_TYPE;
    }

    public void dump(Path outputDirectory) throws IOException {
        File idOutputFile = outputDirectory.resolve("contributors_with_username_id").toFile();
        File usernameOutputFile = outputDirectory.resolve("contributors_with_username_username").toFile();

        try (FileOutputStream idOutput = new FileOutputStream(idOutputFile);
             FileOutputStream usernameOutput = new FileOutputStream(usernameOutputFile)) {
            for (Contributor contributor : contributors) {
                idOutput.write(ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(contributor.id).array());

                usernameOutput.write(contributor.username.getBytes(StandardCharsets.UTF_8));
                usernameOutput.write(NULL_TERMINATOR);
            }
        }
    }
}
