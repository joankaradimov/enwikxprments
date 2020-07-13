#pragma once

#include <algorithm>
#include <array>
#include <vector>

union Float {
    Float() {}
    Float(float f) : f(f) {}

    float f;
    struct {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        signed int sign : 1;
    };
};

template <int CATEGORY_COUNT>
class Ranges {
public:
    Ranges() {}

	void load_ranges(const float* ranges) {
        const Float* f_ranges = reinterpret_cast<const Float*>(ranges);
        unsigned min_exponent = -1;

        for (int i = 0; i < CATEGORY_COUNT; i++) {
            min_exponent = std::min(min_exponent, f_ranges[i].exponent);
        }

        category_ranges[0] = 0;

        for (int i = 0; i < CATEGORY_COUNT; i++) {
            int exponent = (int) f_ranges[i].exponent - min_exponent - 32;
            unsigned __int64 range_length = (1 << 23) | f_ranges[i].mantissa;
            range_length = exponent > 0 ? range_length << exponent : range_length >> -exponent;
            
            category_ranges[i + 1] = category_ranges[i] + range_length;
        }
	}

    unsigned __int64 start(int category) const {
        return category_ranges[category];
    }

    unsigned __int64 length(int category) const {
        return end(category) - start(category);
    }

    unsigned __int64 end(int category) const {
        return category_ranges[category + 1];
    }

    unsigned __int64 total() const {
        return category_ranges[CATEGORY_COUNT];
    }
private:
    unsigned __int64 category_ranges[CATEGORY_COUNT + 1];
};
