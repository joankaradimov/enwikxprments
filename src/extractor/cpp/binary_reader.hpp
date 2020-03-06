#pragma once

#include <fstream>
#include <vector>

class BinaryReader {
public:
    BinaryReader(const char* filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        index = 0;
        data = std::vector<char>(size);
        file.read(data.data(), size);
    }

    template <typename T>
    T read() {
        char* read_start = data.data() + index;
        T result = *((T*) read_start);
        index += sizeof(T);

        return result;
    }

    char* read_string() {
        char* read_start = data.data() + index;
        int length = strlen(read_start) + 1;
        char* result = new char[length];
        memcpy(result, read_start, length);
        index += length;

        return result;
    }

    bool has_more() {
        return index < data.size();
    }
private:
    std::vector<char> data;
    int index;
};
