#include <iostream>  // clang-format off

#include "metastring.hpp"
// Compiling on gcc 9.0.1 with:
//   
// g++ example.cpp -std=c++2a -o example.out -O3 -pedantic -Wall 


// This mini library provides interfaces for converting strings to types and 
// working with those types at compile time.
// 
// The meta::string type is essentially a variadic char template i.e.
//
// template <char... args> struct meta::string;
//
// Therefore any operations can be performed on them using variadic template
// meta programming, this < 150 loc library does not aspire to provide many of
// these operations as they can be easily implemented in the standard way.
//
// Instead the library focuses on construction meta::string types and makes
// passing strings as template parameters easy.



// To build your own templates that accepts literal strings like
// meta::stom_t, you can use the meta::wrap and meta::unwrap helpers:

template <meta::wrap Str>
struct my_class {
    // my_class holds the meta::string type
    using type = meta::unwrap_t<Str>;

    // my_class holds a meta::string instance.
    static constexpr meta::string value = meta::unwrap_v<Str>;
};

// my_class can now be called as: my_class<"some string">, see the bottom of
// this file for more details.


int main() {
    // Firstly you can store a string as a type using stom_t
    // (string-to-meta-type):
    using type_string = meta::stom_t<"abcd">;

    //--$ type_string = meta::string<'a', 'b', 'c', 'd'>


    // Alternatively you can use an constexpr char array.
    constexpr char char_s[] = "hi";

    std::cout << meta::stom_v<char_s> << std::endl; //--$ hi


    // You can slice the type to obtain the characters:
    constexpr auto char_a = type_string::c_str[0];

    std::cout << char_a << std::endl;  //--$ a


    // You can find the length of the string:
    constexpr std::size_t len = type_string::len();

    std::cout << len << std::endl;  //--$ 4


    // You could also convert an integer to a meta::string type using itom_t
    // (integer-to-meta-type):
    using int_string = meta::itom_t<-8>;

    //--$ int_string = meta::string<'-', '8'>

    std::cout << int_string{} << std::endl;  //--$ -8


    // You can instantiate your string types:
    constexpr meta::string instance = type_string{};


    // And then implicitly convert it to a c_style string:
    std::cout << instance << std::endl;  //--$ abcd

    // or explicitly:
    constexpr char const* c_style = instance.c_str;

    std::cout << c_style << std::endl;  //--$ abcd

    // The implicit conversion to a c style string makes working with it at run 
    // time or in a constexpr functions trivial.


    // You can perform compile time string comparisons:
    constexpr int cmp =
        meta::compare(meta::stom_v<"less">, meta::stom_v<"more">);

    std::cout << cmp << std::endl;  //--$ -1


    // And you can perform compile time string concatenation
    constexpr meta::string cat = instance + meta::stom_v<"!">;

    std::cout << cat << std::endl;  //--$ abcd!


    // You can use this library to make templates accepting string literals
    // see the definition of my_class above main().
    using custom_t = my_class<"some str">;

    //--$ custom_t::type = meta::string<'s', 'o', 'm', 'e', ' ', 's', 't', 'r'>

    constexpr custom_t custom;

    std::cout << custom.value << std::endl;  //--$ some str


    // You can even use meta strings instead of string literals as template
    // arguments:
    constexpr auto custom_2 = my_class<instance>{};

    std::cout << custom_2.value << std::endl;  //--$ some abcd



    return 0;
}
