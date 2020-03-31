package com.github.joankaradimov;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.LinkedHashMap;
import java.util.Objects;

public class ContributorsWithIpString {
    public static final class Contributor {
        public Contributor(String ip) {
            this.ip = ip;
        }

        public final String ip;

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Contributor that = (Contributor) o;
            return Objects.equals(ip, that.ip);
        }

        @Override
        public int hashCode() {
            return Objects.hash(ip);
        }
    }

    public static final int CONTRIBUTOR_TYPE = 2; // TODO: use an enum
    private static byte[] NULL_TERMINATOR = new byte[] { 0 };

    private int maxIndex = 0;
    private final LinkedHashMap<Contributor, Integer> contributors = new LinkedHashMap<>();

    public void add(Contributor contributor) {
        if (!contributors.containsKey(contributor)) {
            contributors.put(contributor, maxIndex++);
        }
    }

    public int getIndex(Contributor contributor) {
        return (contributors.get(contributor) << 2) | CONTRIBUTOR_TYPE;
    }

    public void dump(Path outputDirectory) throws IOException {
        File outputFile = outputDirectory.resolve("contributors_with_ip_string").toFile();
        try (FileOutputStream output = new FileOutputStream(outputFile)) {
            for (Contributor contributor : contributors.keySet()) {
                output.write(contributor.ip.getBytes(StandardCharsets.UTF_8));
                output.write(NULL_TERMINATOR);
            }
        }
    }
}
