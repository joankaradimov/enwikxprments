#pragma once

#include <algorithm>
#include <stdlib.h>
#include <intrin.h>
#include <iostream>

template <typename Digit = unsigned long long> class DigitBuffer {
private:
    struct DigitProxy {
        DigitProxy(DigitBuffer& digits_buffer, unsigned index) : digits_buffer(digits_buffer), index(index) {}

        operator Digit() const {
            if (index >= digits_buffer.size)
                return digits_buffer.sign() ? ~0 : 0;
            return digits_buffer.at(index);
        }

        Digit operator=(Digit digit) {
            if (digits_buffer.size <= index) {
                /*if (digits_buffer.size <= indexdigit == (digits_buffer.sign() ? ~0 : 0)) {
                    return digit;
                }*/
                digits_buffer.set_size(index + 1);
            }
            return digits_buffer.at(index) = digit;
        }

        DigitProxy& operator++() {
            ++digits_buffer.at(index);
            return *this;
        }

        DigitProxy& operator--() {
            --digits_buffer.at(index);
            return *this;
        }

        bool operator==(Digit other) const {
            return digits_buffer.at(index) == other;
        }

        bool operator!=(Digit other) const {
            return digits_buffer.at(index) != other;
        }

        DigitBuffer& digits_buffer;
        unsigned index;
    };

public:
    DigitBuffer(std::initializer_list<Digit> l) {
        size = l.size();
        if (size > IMMEDIATE_BUFFER_SIZE) {
            buffer = new Digit[l.size()];
        }
        int i = 0;
        for (Digit digit : l) {
            at(i++) = digit;
        }
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> DigitBuffer(T number) {
        Digit digits[max_digit_count<T>];
        for (unsigned i = 0; i < max_digit_count<T>; i++) {
            digits[i] = Digit(number >> (i * BITS_PER_DIGIT));
        }
        if (std::is_unsigned_v<T>) {
            digits[max_digit_count<T> -1] = 0;
        }

        unsigned meaningful_digits;
        for (meaningful_digits = max_digit_count<T>; meaningful_digits > 1; meaningful_digits--) {
            Digit higher = digits[meaningful_digits - 1];
            if (higher != Digit(~0) && higher != Digit(0)) {
                break;
            }

            bool lower_is_negative = digits[meaningful_digits - 2] & (Digit(1) << (BITS_PER_DIGIT - 1));
            if (higher == Digit(0) && lower_is_negative || higher == Digit(~0) && !lower_is_negative) {
                break;
            }
        }

        size = meaningful_digits;
        if (size > IMMEDIATE_BUFFER_SIZE) {
            buffer = new Digit[size];
        }
        for (unsigned i = 0; i < size; i++) {
            at(i) = digits[i];
        }
    }

    DigitBuffer(const DigitBuffer& other) {
        size = other.size;
        if (size > IMMEDIATE_BUFFER_SIZE) {
            buffer = new Digit[size];
        }
        for (unsigned i = 0; i < size; i++) {
            at(i) = other[i];
        }
        trim();
    }

    ~DigitBuffer() {
        if (size > IMMEDIATE_BUFFER_SIZE) {
            delete[] buffer;
        }
    }

    DigitBuffer& operator=(const DigitBuffer& other) {
        if (this != &other) {
            DigitBuffer temporary = other;
            swap(temporary);
        }
        return *this;
    }

    void set_size(unsigned new_size) {
        DigitBuffer old_digits = {};
        swap(old_digits);

        size = new_size;
        if (size > IMMEDIATE_BUFFER_SIZE) {
            buffer = new Digit[size];
        }

        for (unsigned i = 0; i < size; i++) {
            at(i) = old_digits[i];
        }
    }

    unsigned get_size() const {
        return size;
    }

    DigitProxy operator[](unsigned index) {
        return DigitProxy(*this, index);
    }

    Digit operator[](unsigned index) const {
        if (index >= size) {
            return sign() ? ~0 : 0;
        }
        else if (size <= IMMEDIATE_BUFFER_SIZE) {
            return immediate_buffer[index];
        }
        else {
            return buffer[index];
        }
    }

    void trim() {
        Digit s = sign() ? ~0 : 0;
        unsigned i;
        for (i = 0; i < size - 1 && (*this)[size - i - 1] == s; ++i) {
            if ((s != 0 && oldest_bit(size - i - 2) == 0) || (s == 0 && oldest_bit(size - i - 2) != 0)) {
                break;
            }
        }
        set_size(size - i);
    }

    bool sign() const {
        return oldest_bit(size - 1);
    }

    const DigitBuffer slice(unsigned start, unsigned size) const {
        DigitBuffer result = { 0 };
        result.buffer = this->size > IMMEDIATE_BUFFER_SIZE ? buffer + start : buffer;
        result.size = size;

        return result;
    }

    const static constexpr size_t BITS_PER_DIGIT = sizeof(Digit) * 8;

private:

    void swap(DigitBuffer& other) {
        std::swap(buffer, other.buffer);
        std::swap(size, other.size);
    }

    Digit& at(unsigned index) {
        return size <= IMMEDIATE_BUFFER_SIZE ? immediate_buffer[index] : buffer[index];
    }

    bool oldest_bit(unsigned digit_index) const {
        return !!((*this)[digit_index] & (Digit(1) << (BITS_PER_DIGIT - 1)));
    }

    template <typename T> constexpr const static unsigned max_digit_count = std::max<unsigned>(1, sizeof(T) / sizeof(Digit)) + (std::is_unsigned_v<T> ? 1 : 0);

    const static constexpr unsigned IMMEDIATE_BUFFER_SIZE = sizeof(Digit*) / sizeof(Digit);

    union {
        Digit* buffer;
        Digit immediate_buffer[IMMEDIATE_BUFFER_SIZE];
    };
    unsigned size;
};

template <typename T> struct doubled_size {};
template <> struct doubled_size<uint8_t> { typedef uint16_t type; };
template <> struct doubled_size<uint16_t> { typedef uint32_t type; };
template <> struct doubled_size<uint32_t> { typedef uint64_t type; };

template <typename T> std::pair<T, T> multiply_with_carry(T a, T b) {
    typename doubled_size<T>::type result = a;
    result *= b;
    return std::pair<T, T>(result >> (sizeof(T) * 8), result);
}

template <> inline std::pair<uint64_t, uint64_t> multiply_with_carry(uint64_t a, uint64_t b) {
    uint64_t result_high;
    uint64_t result_low = _umul128(a, b, &result_high);
    return std::pair<uint64_t, uint64_t>(result_high, result_low);
}

template <typename T> std::pair<uint8_t, T> add_with_carry(uint8_t carry, T a, T b) {
    typename doubled_size<T>::type result = carry;
    result += a;
    result += b;
    return std::pair<uint8_t, T>(result >> (sizeof(T) * 8), result);
}

template <> inline std::pair<uint8_t, uint64_t> add_with_carry(uint8_t carry, uint64_t a, uint64_t b) {
    uint64_t result_low;
    carry = _addcarryx_u64(carry, a, b, &result_low);
    return std::pair<uint8_t, uint64_t>(carry, result_low);
}

template <> inline std::pair<uint8_t, uint32_t> add_with_carry(uint8_t carry, uint32_t a, uint32_t b) {
    uint32_t result_low;
    carry = _addcarryx_u32(carry, a, b, &result_low);
    return std::pair<uint8_t, uint32_t>(carry, result_low);
}

template <> inline std::pair<uint8_t, uint16_t> add_with_carry(uint8_t carry, uint16_t a, uint16_t b) {
    uint16_t result_low;
    carry = _addcarry_u16(carry, a, b, &result_low);
    return std::pair<uint8_t, uint16_t>(carry, result_low);
}

template <typename T> std::pair<T, T> udivmod(T high_dividend, T low_dividend, T divisor) {
    typename doubled_size<T>::type dividend = (doubled_size<T>::type(high_dividend) << (sizeof(T) * 8)) | low_dividend;
    return std::pair<T, T>(dividend / divisor, dividend % divisor);
}

template <> inline std::pair<uint64_t, uint64_t> udivmod(uint64_t high_dividend, uint64_t low_dividend, uint64_t divisor) {
    uint64_t remainder;
    uint64_t result = _udiv128(high_dividend, low_dividend, divisor, &remainder);
    return std::pair<uint64_t, uint64_t>(result, remainder);
}

template <> inline std::pair<uint32_t, uint32_t> udivmod(uint32_t high_dividend, uint32_t low_dividend, uint32_t divisor) {
    uint32_t remainder;
    uint64_t dividend = (uint64_t(high_dividend) << 32) | low_dividend;
    uint32_t result = _udiv64(dividend, divisor, &remainder);
    return std::pair<uint32_t, uint32_t>(result, remainder);
}

template <typename T> T shift_left(T high, T low, uint8_t shift) {
    return (doubled_size<T>::type(low) >> (sizeof(T) * 8 - shift)) | (doubled_size<T>::type(high) << shift);
}

template <> inline uint64_t shift_left(uint64_t high, uint64_t low, uint8_t shift) {
    return __shiftleft128(low, high, shift);
}

template <typename T> T shift_right(T high, T low, uint8_t shift) {
    return (doubled_size<T>::type(low) >> shift) | (doubled_size<T>::type(high) << (sizeof(T) * 8 - shift));
}

template <> inline uint64_t shift_right(uint64_t high, uint64_t low, uint8_t shift) {
    return __shiftright128(low, high, shift);
}

template <typename Digit = unsigned long long> class BigInteger
{
public:
    BigInteger() : digits({ 0 }) { }

    BigInteger(std::pair<Digit, Digit> num) : digits({ num.second, num.first, 0 }) { }

    BigInteger(const BigInteger& other) : digits(other.digits) { }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger(T number) : digits(number) { }

    BigInteger(const char* str) : BigInteger() {
        bool is_negative = false;
        int base = 10;

        if (*str == '+') {
            ++str;
        }
        else if (*str == '-') {
            is_negative = true;
            ++str;
        }

        if (str[0] == '0') {
            if (str[1] == 'x') {
                base = 16;
                str += 2;
            }
            else if (str[1] == 'b') {
                base = 2;
                str += 2;
            }
            else {
                base = 8;
                str += 1;
            }
        }

        for (int i = 0; str[i]; ++i) {
            // TODO: read multiple decimal digits at once
            (*this) *= base;
            (*this) += char_to_digit(str[i]);
        }

        if (is_negative) {
            *this = -(*this);
        }
    }

    BigInteger operator~() const {
        BigInteger res(*this);
        for (unsigned i = 0; i < res.digits.get_size(); ++i)
            res.digits[i] = ~res.digits[i];
        return res;
    }

    BigInteger operator-() const {
        BigInteger res = ~*this;
        return ++res;
    }

    const BigInteger& operator+() const {
        return *this;
    }

    BigInteger operator+(const BigInteger& r) const {
        int m = std::max(digits.get_size(), r.digits.get_size());
        int carry_size = digits.sign() == r.digits.sign();
        BigInteger res;
        res.digits.set_size(m + carry_size);
        unsigned __int8 carry = 0;
        for (unsigned i = 0; i < res.digits.get_size(); ++i) {
            auto result = add_with_carry(carry, digits[i], r.digits[i]);
            carry = result.first;
            res.digits[i] = result.second;
        }
        return res;
    }

    BigInteger operator+(Digit other) const {
        BigInteger result = *this;
        return result += other;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger operator+(T other) const {
        return *this + BigInteger(other);
    }

    BigInteger& operator+=(const BigInteger& r) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) + r;
        return *this;
    }

    BigInteger& operator+=(Digit other) {
        bool was_negative = digits.sign();
        auto result = add_with_carry<Digit>(0, digits[0], other);
        uint8_t carry = result.first;
        digits[0] = result.second;

        for (unsigned i = 1; i < digits.get_size() && carry; ++i) {
            ++digits[i];
            carry = (digits[i] == Digit(0));
        }

        bool is_negative = digits.sign();
        if (is_negative && !was_negative) {
            digits[digits.get_size()] = 0;
        }
        return *this;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger& operator+=(T other) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) + other;
        return *this;
    }

    BigInteger operator-(const BigInteger& r) const {
        return (*this) + (-r);
    }

    BigInteger operator-(Digit other) const {
        BigInteger result = *this;
        result -= other;
        return result;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger operator-(T other) const {
        return *this - BigInteger(other);
    }

    BigInteger& operator-=(const BigInteger& r) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) - r;
        return *this;
    }

    BigInteger& operator-=(Digit other) {
        bool was_negative = digits.sign();
        auto result = add_with_carry<Digit>(0, digits[0], -other);
        uint8_t carry = !result.first;
        digits[0] = result.second;

        for (unsigned i = 1; i < digits.get_size() && carry; ++i) {
            --digits[i];
            carry = (digits[i] == Digit(~0));
        }

        bool is_negative = digits.sign();
        if (!is_negative && was_negative) {
            digits[digits.get_size()] = Digit(~0);
        }
        return *this;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger& operator-=(T other) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) - other;
        return *this;
    }

    BigInteger operator*(const BigInteger& lnum) const {
        if (digits.sign() && lnum.digits.sign()) {
            return (-*this).umul(-lnum);
        }
        else if (digits.sign()) {
            return -lnum.umul(-*this);
        }
        else if (lnum.digits.sign()) {
            return -umul(-lnum);
        }
        else {
            return umul(lnum);
        }
    }

    BigInteger operator*(Digit r) const {
        BigInteger res;
        bool s = digits.sign();
        BigInteger l_positive = abs();
        res.digits.set_size(l_positive.digits.get_size() + 1);
        for (unsigned i = 0; i < l_positive.digits.get_size(); i++) {
            BigInteger t = multiply_with_carry<Digit>(l_positive.digits[i], r);
            res += t << (i * digits.BITS_PER_DIGIT);
        }
        return s ? -res : res;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger operator*(T other) const {
        return *this * BigInteger(other);
    }

    BigInteger& operator*=(const BigInteger& r) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) * r;
        return *this;
    }

    BigInteger& operator*=(Digit r) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) * r;
        return *this;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger& operator*=(T other) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) * other;
        return *this;
    }

    BigInteger operator/(const BigInteger& lnum) const {
        int s = digits.sign() ^ lnum.digits.sign();
        BigInteger result = abs().divmod(lnum.abs()).first;

        return s ? -result : result;
    }

    BigInteger operator/(Digit other) const {
        int s = digits.sign();
        auto result = abs().divmod(other).first;

        return s ? -result : result;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger operator/(T other) const {
        return *this / BigInteger(other);
    }

    BigInteger& operator/=(const BigInteger& r) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) / r;
        return *this;
    }

    BigInteger& operator/=(Digit r) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) / r;
        return *this;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger& operator/=(T other) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) / other;
        return *this;
    }

    BigInteger operator%(const BigInteger& lnum) const {
        int s = digits.sign() ^ lnum.digits.sign();
        BigInteger result = abs().divmod(lnum.abs()).second;

        return s ? -result : result;
    }

    BigInteger operator%(Digit other) const {
        int s = digits.sign();
        auto result = abs().divmod(other).second;

        return s ? -BigInteger(result) : result;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger operator%(T other) const {
        return *this % BigInteger(other);
    }

    BigInteger& operator%=(const BigInteger& r) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) % r;
        return *this;
    }

    BigInteger& operator%=(Digit r) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) % r;
        return *this;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger& operator%=(T other) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) % other;
        return *this;
    }

    bool operator<(const BigInteger& other) const {
        if (digits.sign() != other.digits.sign()) {
            return digits.sign();
        }

        // TODO: optimize -- do not iterate numbers of different sizes
        for (int i = std::max(digits.get_size(), other.digits.get_size()) - 1; i >= 0; i--) {
            if (digits[i] < other.digits[i]) {
                return true;
            }
            if (digits[i] > other.digits[i]) {
                return false;
            }
        }
        return false;
    }

    bool operator>(const BigInteger& r) const {
        return r < *this;
    }

    bool operator<=(const BigInteger& r) const {
        return !(*this > r);
    }

    bool operator>=(const BigInteger& r) const {
        return !(*this < r);
    }

    bool operator==(const BigInteger& other) const {
        // TODO: optimize -- do not iterate numbers of different sizes
        for (unsigned i = 0; i < std::max(digits.get_size(), other.digits.get_size()); i++) {
            if (digits[i] != other.digits[i]) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const BigInteger& r) const {
        return !(*this == r);
    }

    const BigInteger& operator=(const BigInteger& number) {
        digits = number.digits;
        return *this;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> const BigInteger& operator=(T number) {
        BigInteger temporary = number;
        std::swap(*this, temporary);
        return *this;
    }

    template<typename T> explicit operator std::basic_string<T>() const {
        if (!*this) {
            return "0";
        }

        std::string result;
        bool is_negative = digits.sign();
        BigInteger temporary = abs();

        for (int i = 0; temporary; ++i) {
            auto divmod_result = temporary.divmod(BigInteger::IO_BASE);
            result.push_back('0' + T(divmod_result.second));
            temporary = divmod_result.first;
        }

        if (is_negative) {
            result.push_back('-');
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

    explicit operator long double() const {
        long double result = 0.0;
        long double multiplier = pow(2.0, digits.BITS_PER_DIGIT);
        BigInteger positive = abs();

        for (int i = positive.digits.get_size() - 1; i >= 0; i--) {
            result *= multiplier;
            result += positive.digits[i];
        }
        return digits.sign() ? -result : result;
    }

    explicit operator Digit() const {
        return digits[0];
    }

    explicit operator bool() const {
        for (unsigned i = 0; i < digits.get_size(); i++) {
            if (digits[i] != 0) {
                return true;
            }
        }

        return false;
    }

    BigInteger operator++(int) {
        BigInteger old = *this;
        (*this) += Digit(1);
        return old;
    }

    BigInteger& operator++() {
        return *this += Digit(1);
    }

    BigInteger operator--(int) {
        BigInteger old = *this;
        (*this) -= Digit(1);
        return old;
    }

    BigInteger& operator--() {
        return *this -= Digit(1);
    }

    // TODO: implement | ^ |= ^=

    BigInteger operator&(const BigInteger& other) const {
        BigInteger result = *this;
        result &= other;
        return result;
    }

    Digit operator&(Digit other) const {
        return digits[0] & other;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger operator&(T other) const {
        return *this & BigInteger(other);
    }

    BigInteger& operator&=(const BigInteger& other) {
        digits.set_size(std::min(digits.get_size(), other.digits.get_size()));
        for (unsigned i = 0; i < digits.get_size(); i++) {
            digits[i] = digits[i] & other.digits[i];
        }
        return *this;
    }

    BigInteger& operator&=(Digit other) {
        digits[0] = digits[0] & other;
        digits.set_size(1);
        return *this;
    }

    template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr> BigInteger& operator&=(T other) {
        *this &= BigInteger(other);
        return *this;
    }

    BigInteger operator<<(unsigned shift) const {
        unsigned hword_shift = shift / digits.BITS_PER_DIGIT;
        unsigned bit_shift = shift % digits.BITS_PER_DIGIT;

        BigInteger res;
        for (unsigned i = digits.get_size(); i > 0; --i) {
            res.digits[hword_shift + i] = shift_left(digits[i], digits[i - 1], bit_shift);
        }
        res.digits[hword_shift] = shift_left(digits[0], Digit(0), bit_shift);
        return res;
    }

    BigInteger operator>>(unsigned shift) const {
        unsigned hword_shift = shift / digits.BITS_PER_DIGIT;
        unsigned bit_shift = shift % digits.BITS_PER_DIGIT;

        if (hword_shift >= digits.get_size()) return digits.sign() ? -1 : 0;
        BigInteger res;
        for (unsigned i = digits.get_size() - hword_shift; i > 0; --i) {
            res.digits[i - 1] = shift_right(digits[hword_shift + i], digits[hword_shift + i - 1], bit_shift);
        }
        return res;
    }

    BigInteger& operator<<=(unsigned shift) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) << shift;
        return *this;
    }

    BigInteger& operator>>=(unsigned shift) {
        // TODO: optimize -- do not create extra instances
        *this = (*this) >> shift;
        return *this;
    }

    std::pair<BigInteger, BigInteger> divmod(const BigInteger& other) const {
        // TODO: Can this be optimized by implementing division via multiplication https://research.swtch.com/divmult?
        if (!other) {
            // This will blow up with a CPU error
            return std::pair<BigInteger, BigInteger>(1 / Digit(other), 1 % Digit(other));
        }

        BigInteger scaled_divisor = other;
        BigInteger remain = *this;
        BigInteger result;
        BigInteger multiple = Digit(1);

        while (scaled_divisor < *this) {
            scaled_divisor <<= 1;
            multiple <<= 1;
        }

        do {
            if (remain >= scaled_divisor) {
                remain -= scaled_divisor;
                result += multiple;
            }
            scaled_divisor >>= 1;
            multiple >>= 1;
        } while (multiple);

        return std::pair<BigInteger, BigInteger>(result, remain);
    }

    BigInteger<Digit> umul(const BigInteger& other) const {
        // TODO: implement Karatsuba, maybe?
        BigInteger res;
        for (unsigned i = other.digits.get_size(); i > 0; --i) {
            res += (*this * other.digits[i - 1]) << ((i - 1) * digits.BITS_PER_DIGIT);
        }
        return res;
    }

    std::pair<BigInteger, Digit> divmod(Digit r) const {
        BigInteger result;
        BigInteger remain = *this;

        for (int i = remain.digits.get_size(); i > 0; --i) {
            auto divmod_result = udivmod<Digit>(remain.digits[i], remain.digits[i - 1], r);
            result.digits[i - 1] = divmod_result.first;
            remain.digits[i - 1] = divmod_result.second;
        }
        return std::pair<BigInteger, Digit>(result, remain);
    }

    void printbin() const {
        for (int i = digits.get_size() - 1; i >= 0; --i) {
            for (int bit_index = digits.BITS_PER_DIGIT - 1; bit_index >= 0; bit_index--) {
                Digit mask = Digit(1) << bit_index;
                std::cout << ((digits[i] & mask) ? 1 : 0);
            }
        }
    }

    void printhex() const {
        for (int i = digits.get_size() - 1; i >= 0; --i) {
            std::cout << std::hex << digits[i];
        }
    }

    BigInteger abs() const {
        return digits.sign() ? -*this : *this;
    }

    typedef Digit DigitType;
    static const Digit IO_BASE = 10;

    // private:
    const BigInteger slice(unsigned start, unsigned size) const {
        BigInteger result;
        result.digits = digits.slice(start, size);
        return result;
    }

    static Digit char_to_digit(char c) {
        if ('0' <= c && c <= '9') {
            return (Digit)c - '0';
        }
        if ('a' <= c && c <= 'z') {
            return (Digit)c - 'a' + 0xA;
        }
        if ('A' <= c && c <= 'Z') {
            return (Digit)c - 'A' + 0xA;
        }
        throw std::invalid_argument((std::string) "Cannot conver character '" + c + "' to digit");
    }

    DigitBuffer<Digit> digits;
};

template <typename T, typename Digit> BigInteger<Digit> operator+(T l, const BigInteger<Digit>& r) {
    return r + l;
}

template <typename T, typename Digit> BigInteger<Digit> operator-(T l, const BigInteger<Digit>& r) {
    return (-r) + l;
}

template <typename T, typename Digit> BigInteger<Digit> operator*(T l, const BigInteger<Digit>& r) {
    return r * l;
}

template <typename T, typename Digit> BigInteger<Digit> operator/(T l, const BigInteger<Digit>& r) {
    return BigInteger<Digit>(l) / r;
}

template <typename T, typename Digit> BigInteger<Digit> operator%(T l, const BigInteger<Digit>& r) {
    return BigInteger<Digit>(l) % r;
}

template <typename T, typename Digit> bool operator<(T l, const BigInteger<Digit>& r) {
    return r > l;
}

template <typename T, typename Digit> bool operator>(T l, const BigInteger<Digit>& r) {
    return r < l;
}

template <typename T, typename Digit> bool operator<=(T l, const BigInteger<Digit>& r) {
    return r >= l;
}

template <typename T, typename Digit> bool operator>=(T l, const BigInteger<Digit>& r) {
    return r <= l;
}

template <typename T, typename Digit> bool operator==(T l, const BigInteger<Digit>& r) {
    return r == l;
}

template <typename T, typename Digit> bool operator!=(T l, const BigInteger<Digit>& r) {
    return r != l;
}

template <typename T, typename Digit> std::basic_ostream<T>& operator<<(std::basic_ostream<T>& os, const BigInteger<Digit>& number) {
    return os << std::string(number);
}

template <typename T, typename Digit> std::basic_istream<T>& operator>>(std::basic_istream<T>& is, BigInteger<Digit>& ln) {
    ln = 0;
    int is_negative = false;
    T ch;
    is >> ch;
    if (ch == '-') {
        is_negative = true;
        is >> ch;
    }
    else if (ch == '+') {
        is >> ch;
    }

    for (;;) {
        ln *= BigInteger<Digit>::IO_BASE;
        ln += ch - '0';
        ch = is.peek();
        if (ch < '0' || ch > '9' || isspace(ch)) break;
        is >> ch;
    }
    if (is_negative) ln = -ln;
    return is;
}
