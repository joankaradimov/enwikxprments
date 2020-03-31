package com.github.joankaradimov;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.*;

public class ContributorsWithIpAddress {
    public static final class Contributor {
        public Contributor(String ip) {
            this.ipAddress = new byte[4];
            int index = 0;
            for (String component : ip.split("\\.")) {
                int number = Integer.valueOf(component);
                ipAddress[index++] = (byte) (number & 0xFF);
            }
        }

        public final byte[] ipAddress;

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Contributor that = (Contributor) o;
            return Arrays.equals(ipAddress, that.ipAddress);
        }

        @Override
        public int hashCode() {
            return Arrays.hashCode(ipAddress);
        }
    }

    public static final int CONTRIBUTOR_TYPE = 0; // TODO: use an enum

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
        File outputFile = outputDirectory.resolve("contributors_with_ip_address").toFile();
        try (FileOutputStream output = new FileOutputStream(outputFile)) {
            for (Contributor contributor : contributorList) {
                output.write(contributor.ipAddress);
            }
        }
    }
}
