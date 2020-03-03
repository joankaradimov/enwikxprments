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
            return Integer.compare(this.id, other.id);
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Contributor that = (Contributor) o;
            return id == that.id;
        }

        @Override
        public int hashCode() {
            return Objects.hash(id);
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
        File outputFile = outputDirectory.resolve("contributors_with_username").toFile();
        try (FileOutputStream output = new FileOutputStream(outputFile)) {
            for (Contributor contributor : contributors) {
                output.write(ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(contributor.id).array());
                output.write(contributor.username.getBytes(StandardCharsets.UTF_8));
                output.write(NULL_TERMINATOR);
            }
        }
    }
}
