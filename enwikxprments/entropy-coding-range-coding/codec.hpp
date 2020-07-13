#pragma once

#include <algorithm>

#include "biginteger.hpp"
#define bignum BigInteger<>

#include "ranges.hpp"

template <typename T> T gcd1(T a, T b) {
    while (b) {
        T t = b;
        b = a % b;
        a = t;
    }
    return a;
}

template <typename T> T gcd(T u, T v) {
    unsigned int shift = 0;

    if (!u) return v;
    if (!v) return u;

    while ((u & 1) == 0 && (v & 1) == 0) {
        shift++;
        u >>= 1;
        v >>= 1;
    }

    while ((u & 1) == 0) {
        u >>= 1;
    }

    do {
        while ((v & 1) == 0) {
            v >>= 1;
        }

        if (u > v) {
            std::swap(u, v);
        }

        v -= u;
    } while (v);

    return u << shift;
}

template <int _CATEGORY_COUNT>
class Codec {
public:
    Codec() {
        codec_start = 0;
        codec_length = 1;
        codec_denominator = 1;
    }

    void load_ranges(float* ranges) {
        this->ranges.load_ranges(ranges);
    }

    void renormalize(int category) {
        auto ranges_denominator = ranges.total();
        codec_start *= ranges_denominator;
        codec_start -= codec_denominator * this->ranges.start(category);
        codec_length *= ranges_denominator;
        codec_denominator *= this->ranges.length(category);

        //std::cout << codec_start << std::endl;
        //std::cout << codec_length << std::endl;
        //std::cout << codec_denominator << std::endl;

        // auto common_denominator = gcd(gcd(codec_start.abs(), codec_length), codec_denominator);
        // codec_start /= common_denominator;
        // codec_length /= common_denominator;
        // codec_denominator /= common_denominator;
    }

    void step(bool bit) {
        if ((codec_length & bignum::DigitType(1)) == 1) {
            codec_start <<= 1;
            codec_denominator <<= 1;

            /*if (Digit(codec_denominator) == 0) {
                codec_start >>= (sizeof(Digit) * 8);
                codec_length >>= (sizeof(Digit) * 8);
                codec_denominator >>= (sizeof(Digit) * 8);
            }*/
        }
        else {
            codec_length >>= 1;
        }

        if (bit) {
            codec_start += codec_length;
        }
    }

protected:
    bignum codec_start;
    bignum codec_length;
    bignum codec_denominator;

    Ranges<_CATEGORY_COUNT> ranges;
};

template <int _CATEGORY_COUNT>
class Encoder: public Codec< _CATEGORY_COUNT> {
public:
    Encoder() {
    }

    void step(bool bit) {
        Codec< _CATEGORY_COUNT>::step(bit);
        encoded_bits.push_back(bit);
    }

    void encode(short category) {
        while (true) {
            auto codec_middle = this->ranges.total() * (2 * this->codec_start + this->codec_length);

            if (2 * this->ranges.end(category) * this->codec_denominator < codec_middle) {
                step(0);
            }
            else if (codec_middle <= 2 * this->ranges.start(category) * this->codec_denominator) {
                step(1);
            }
            else {
                this->renormalize(category);
                return;
            }
        }
    }

    void encode_end(short category) {
        while (this->codec_start * this->ranges.total() < this->ranges.start(category) * this->codec_denominator && this->ranges.end(category) * this->codec_denominator < (this->codec_start + this->codec_length) * this->ranges.total()) {
            if (2 * (this->ranges.start(category) + this->ranges.end(category)) * this->codec_denominator > this->ranges.total() * (2 * this->codec_start + this->codec_length)) {
                step(1);
            }
            else {
                step(0);
            }
        }
    }

    const std::vector<bool>& data() {
        return encoded_bits;
    }

private:
    std::vector<bool> encoded_bits;
};

template <int _CATEGORY_COUNT>
class Decoder : public Codec< _CATEGORY_COUNT> {
public:
    Decoder() {
    }

    void load_ranges(float* ranges) {
        Codec< _CATEGORY_COUNT>::load_ranges(ranges);
        decoder_start_category = 0;
        decoder_end_category = _CATEGORY_COUNT - 1;
        decoder_start_category = find_category(this->codec_start);
        decoder_end_category = find_category(this->codec_start + this->codec_length);
    }

    void decode(bool bit) {
        this->step(bit);

        if (bit) {
            decoder_start_category = find_category(this->codec_start);
        }
        else {
            decoder_end_category = find_category(this->codec_start + this->codec_length);
        }

        if (decoder_start_category == decoder_end_category) {
            this->renormalize(decoder_start_category);
            decoded_catergories.push_back(decoder_start_category);
        }
    }

    const std::vector<short>& data() {
        return decoded_catergories;
    }

    short find_category(const bignum& target_range) {
        short start = decoder_start_category;
        short end = decoder_end_category;

        bignum scaled_target_range = target_range * this->ranges.total();

        while (start <= end) {
            int middle = (start + end) / 2;

            /* TODO: aim for the approximate category
            bignum category_delta = (this->ranges.end(end) - this->ranges.start(start)) * this->codec_denominator;
            bignum target_delta = target_range * this->ranges.total() - this->ranges.start(start) * this->codec_denominator;
            short middle = ((end - start) * target_delta / category_delta).to_int();
            middle = middle <= start ? start + 1 : middle < end ? middle : end - 1;
            /**/

            if (this->codec_denominator * this->ranges.end(middle) <= scaled_target_range) {
                start = middle + 1;
            }
            else {
                end = middle - 1;
            }
        }
        return start;
    }

private:
    short decoder_start_category;
    short decoder_end_category;

    std::vector<short> decoded_catergories;
};
