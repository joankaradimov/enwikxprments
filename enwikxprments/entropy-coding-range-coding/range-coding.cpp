#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <vector>

#include <string>

#include "codec.hpp"

#define CATEGORY_COUNT 255

int main()
{
    float f = 1.5;

    float(*weights)[CATEGORY_COUNT] = new float[1100][CATEGORY_COUNT];
    short* actual = new short[1100];
    int length = 0;
    FILE* predictions_file = fopen("C:\\Users\\joank_000\\Desktop\\rnn-enwik-predictor\\char-predictions", "rb");
    FILE* actual_file = fopen("C:\\Users\\joank_000\\Desktop\\rnn-enwik-predictor\\char-actual", "rb");

    while (fread(weights + length, sizeof(float), CATEGORY_COUNT, predictions_file) > 0) {
        fread(actual + length, sizeof(short), 1, actual_file);
        length++;
    }

    fclose(predictions_file);
    fclose(actual_file);

    std::chrono::steady_clock::time_point start_time, end_time;
    std::chrono::steady_clock::duration time;
    double miliseconds;

    start_time = std::chrono::high_resolution_clock::now();
    Encoder<CATEGORY_COUNT> encoder;
    for (int i = 0; i < 250; i++) {
        encoder.load_ranges(weights[i]);
        encoder.encode(actual[i]);
    }
    // encoder.encode_end(actual[100]);
    end_time = std::chrono::high_resolution_clock::now();
    time = end_time - start_time;
    miliseconds = time / std::chrono::milliseconds(1);
    printf("Took %lf ms.\n", miliseconds);
    printf("Compressed length: %d\n", encoder.data().size());

    start_time = std::chrono::high_resolution_clock::now();
    Decoder<CATEGORY_COUNT> decoder;
    int j = 0;
    int last_decoded_size = -1;
    for (int i = 0; i < encoder.data().size(); i++) {
        if (last_decoded_size != decoder.data().size()) {
            decoder.load_ranges(weights[j++]);
            last_decoded_size = decoder.data().size();
        }
        decoder.decode(encoder.data()[i]);
    }
    end_time = std::chrono::high_resolution_clock::now();
    time = end_time - start_time;
    miliseconds = time / std::chrono::milliseconds(1);
    printf("Took %lf ms.\n", miliseconds);
    printf("Decompressed length: %d\n", decoder.data().size());
    // Took 18479.000000 ms.
    // Decompressed length : 10

    for (int i = 0; i < decoder.data().size() - 1; i++) { // TODO: fix the last bit
        if (decoder.data()[i] != actual[i]) {
            puts("ERROR!");
            throw "ERROR";
        }
    }

    /*std::vector<bool> encoded_data;

    start_time = std::chrono::high_resolution_clock::now();

    bignum range_start = 0;
    bignum range_total = 1;
    bignum range_length = 1;

    Ranges<CATEGORY_COUNT> ranges;
    for (int i = 0; i < 10; i++) {
        ranges.load_ranges(weights[i]);
        short actual_category = actual[i];

        auto new_start = ranges.start(actual_category);
        auto new_length = ranges.length(actual_category);
        auto new_total = ranges.total();

        auto denominator = 1; // gcd<bignum>(new_total, range_length).to_long_long();
        auto new_total_quotient = new_total / denominator;
        auto range_length_quotient = range_length / denominator;

        // auto new_total_quotient = new_total;
        // auto range_length_quotient = range_length;

        range_start *= new_total_quotient;
        range_total *= new_total_quotient;
        range_start = range_start + range_length_quotient * new_start;
        range_length = range_length_quotient * new_length;

        while (true) {
            bignum range_middle = range_total / 2;

            if (range_start + range_length < range_middle) {
                range_total = range_middle;

                encoded_data.push_back(0);
            }
            else if (range_start >= range_middle) {
                range_start = range_start - range_middle;
                range_total = range_total - range_middle;

                encoded_data.push_back(1);
            }
            else {
                break;
            }
        }
    }

    end_time = std::chrono::high_resolution_clock::now();
    time = end_time - start_time;
    miliseconds = time / std::chrono::milliseconds(1);
    printf("Took %lf ms.\n", miliseconds);
    printf("Compressed length: %d\n", encoded_data.size());*/
    // First implementation, first 50 items:
    // Took 103459.000000 ms
    // 716

    // Seconnd implementation, first 50 items:
    // Took 480.000000 ms.
    // Compressed length : Compressed length : 716

    // Second implementation, first 50 items:
    // Took Took 456.000000 ms.
    // Compressed length : 716

    // Second implementation , the whole thing:
    // Took 23742959.000000 ms.
    // Compressed length : 114553

    /*range_start = 0;
    range_length = 1;
    range_total = 1;

    std::vector<int> decoded_data;
    int data_index = 0;
    for (int i = 0; i < 20; i++) {
        Ranges<CATEGORY_COUNT> ranges(weights[i]);
        
        range_start *= ranges.total();
        range_length *= ranges.total();

        while (true) {
            if (encoded_data[data_index++]) {
                range_start += range_length / 2;
                range_length = range_length - range_length / 2;
            }
            else {
                range_length = range_length / 2;
            }
            
            int category = ranges.find_category(range_start, range_total);
            if (range_start + range_length < range_total * ranges.end(category)) {
                decoded_data.push_back(category);

                range_start -= range_total * ranges.start(category);
                range_total *= ranges.length(category);

                break;
            }
        }
    }*/

    /*unsigned coefficients[CATEGORY_COUNT];
    for (int i = 0; i < CATEGORY_COUNT; i++) {
        Float weight = w[i].f;

        unsigned exponent = weight.exponent - min_exponent;
        unsigned mantissa = (1 << 23) | weight.mantissa;
        
        coefficients[i] = mantissa << exponent;
    }

    for (int i = 0; i < CATEGORY_COUNT; i++) {
        Float weight = w[i].f;
        float coefficient = coefficients[i];
        long long coefficient_denominator = 1ll << (127 + 23 - min_exponent);

        if (weight.f * coefficient_denominator != coefficients[i]) {
            throw "error";
        }
    }*/

    printf("done\n");
}
