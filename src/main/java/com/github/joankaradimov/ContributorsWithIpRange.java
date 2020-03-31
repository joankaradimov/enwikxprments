package com.github.joankaradimov;

import java.io.*;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

public class ContributorsWithIpRange {
    public static final class Contributor {
        public Contributor(String ip) {
            this.ipRange = new byte[3];
            int index = 0;
            for (String component : ip.replace(".xxx", "").split("\\.")) {
                int number = Integer.valueOf(component);
                ipRange[index++] = (byte) (number & 0xFF);
            }
        }

        public final byte[] ipRange;

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Contributor that = (Contributor) o;
            return Arrays.equals(ipRange, that.ipRange);
        }

        @Override
        public int hashCode() {
            return Arrays.hashCode(ipRange);
        }
    }

    public static final int CONTRIBUTOR_TYPE = 1; // TODO: use an enum

    private int maxIndex = 0;
    private final ArrayList<Contributor> contributorList = new ArrayList<>();
    private final HashMap<Contributor, Integer> contributors = new HashMap<>();

    public void add(Contributor contributor) {
        if (!contributors.containsKey(contributor)) {
            contributors.put(contributor, maxIndex++);
            contributorList.add(contributor);
        }
    }

    public int getIndex(Contributor contributor) {
        return (contributors.get(contributor) << 2) | CONTRIBUTOR_TYPE;
    }

    public void dump(Path outputDirectory) throws IOException {
        File outputFile = outputDirectory.resolve("contributors_with_ip_range").toFile();
        try (FileOutputStream output = new FileOutputStream(outputFile)) {
            for (Contributor contributor : contributorList) {
                output.write(contributor.ipRange);
            }
        }
    }
}
