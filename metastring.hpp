#pragma once

// MIT License

// Copyright (c) 2020 Conor Williams

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdint>

// Provides utilities for working with compile time strings as types and passing
// string literals as template parameters. See the examples for details.

namespace meta {

template <char... args>
struct string {
    static constexpr char c_str[sizeof...(args) + 1] = {args..., '\0'};
    static constexpr std::size_t len() { return sizeof...(args); }
    constexpr operator char const*() const { return c_str; }
};

template <char... lhs, char... rhs>
constexpr auto operator+(string<lhs...> a, string<rhs...> b) {
    return string<lhs..., rhs...>{};
}

template <char... args>
constexpr int compare(string<args...>, string<args...>) {
    return 0;
}

template <char... lhs, char... rhs>
constexpr int compare(string<lhs...> a, string<rhs...> b) {
    if constexpr (sizeof...(lhs) < sizeof...(rhs)) {
        return -1;
    }
    if constexpr (sizeof...(lhs) > sizeof...(rhs)) {
        return 1;
    }
    for (std::size_t i = 0; i < sizeof...(lhs); ++i) {
        if (a.c_str[i] < b.c_str[i]) {
            return -1;
        }
        if (a.c_str[i] > b.c_str[i]) {
            return 1;
        }
    }
}

// Struct to wrap a template non-type parameter string literal
template <std::size_t N>
struct wrap {
    static constexpr std::size_t size() { return N - 1; }

    constexpr wrap(char const* s) {
        for (std::size_t i = 0; i < N - 1; ++i) {
            buf[i] = s[i];
        }
    }

    template <char... args>
    constexpr wrap(string<args...> s) {
        for (std::size_t i = 0; i < N - 1; ++i) {
            buf[i] = s[i];
        }
    }

    char buf[N - 1]{};  // miss null
};

template <unsigned N>
wrap(char const (&)[N])->wrap<N>;

template <char... args>
wrap(string<args...>)->wrap<sizeof...(args) + 1>;

namespace impl {

// Convert a wrap{} into a type as cannot seem to pattern match on user defined
// non template types on gcc 9.0.1
template <auto S>
struct wrap_type {
    static constexpr auto c_str = S;
};

template <typename S, std::size_t N, char... args>
struct explode : explode<S, N - 1, S::c_str.buf[N - 1], args...> {};

template <typename S, char... args>
struct explode<S, 0, args...> {
    using type = string<args...>;
};

constexpr std::intmax_t abs_val(std::intmax_t x) { return x < 0 ? -x : x; }

constexpr std::intmax_t num_digits(std::intmax_t x) {
    return x < 0 ? 1 + num_digits(-x) : x < 10 ? 1 : 1 + num_digits(x / 10);
}

template <std::intmax_t size, std::intmax_t x, char... args>
struct int_to_str_impl
    : int_to_str_impl<size - 1, x / 10, '0' + abs_val(x) % 10, args...> {};

template <std::intmax_t x, char... args>
struct int_to_str_impl<2, x, args...> {
    using type =
        string < x<0 ? '-' : '0' + x / 10, '0' + abs_val(x) % 10, args...>;
};

template <std::intmax_t x, char... args>
struct int_to_str_impl<1, x, args...> {
    using type = string<'0' + x, args...>;
};

}  // namespace impl

template <auto wrapper>
using unwrap_t = impl::explode<impl::wrap_type<wrapper>, wrapper.size()>::type;

template <auto wrapper>
inline constexpr string unwrap_v = unwrap_t<wrapper>{};

template <wrap wrapper>
using stom_t = unwrap_t<wrapper>;

template <wrap wrapper>
inline constexpr string stom_v = unwrap_t<wrapper>{};

template <std::intmax_t x>
using itom_t = impl::int_to_str_impl<impl::num_digits(x), x>::type;

template <std::intmax_t x>
inline constexpr string itom_v = itom_t<x>{};

}  // namespace meta
