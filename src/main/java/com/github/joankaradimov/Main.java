package com.github.joankaradimov;

import java.io.*;

public class Main {
    public static void main(String[] args) {
        try (FileInputStream stream = new FileInputStream("build/enwik8")) {
            ByteArrayInputStream xmlClosingElements = new ByteArrayInputStream("</text></revision></page></mediawiki>".getBytes());
            SequenceInputStream validXmlStream = new SequenceInputStream(stream, xmlClosingElements);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
