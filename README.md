# metastring

This mini library provides interfaces for converting strings to types and working with those types at compile time.

Just inlude the single header and go!

Tested on gcc 9.0.1 with: `g++ example.cpp -std=c++2a -o example.out -O3 -pedantic -Wall`

## Info

The meta::string type is essentially a variadic char template:

```c++
template <char... args> struct meta::string;
```

Therefore any operations can be performed on them using variadic template meta programming, this < 150 loc library does 
not aspire to provide many of these operations as they can be easily implemented in the standard way.

Instead metastring focuses on construction meta::string types and makes passing strings as template parameters to custom 
classes easy.

## Examples

### Meta strings

Firstly you can store a string as a type using stom_t (string-to-meta-type):
```c++
using type_string = meta::stom_t<"abcd">;

//--$ type_string = meta::string<'a', 'b', 'c', 'd'>
```
Alternatively you can use an constexpr char array:
```c++
constexpr char char_s[] = "hi";

std::cout << meta::stom_v<char_s> << std::endl; 

//--$ hi
```
You can slice the type to obtain the characters:
```c++
constexpr auto char_a = type_string::c_str[0];

std::cout << char_a << std::endl;  

//--$ a
```
You can find the length of the string:
```c++
constexpr std::size_t len = type_string::len();

std::cout << len << std::endl;  

//--$ 4
```
You could also convert an integer to a meta::string type using itom_t (integer-to-meta-type):
```c++
using int_string = meta::itom_t<-8>;

//--$ int_string = meta::string<'-', '8'>

std::cout << int_string{} << std::endl;  

//--$ -8
```
You can instantiate your string types:
```c++
constexpr meta::string instance = type_string{};
```
And then implicitly convert it to a c_style string:
```c++
std::cout << instance << std::endl;  

//--$ abcd

// or explicitly:
constexpr char const* c_style = instance.c_str;

std::cout << c_style << std::endl;  

//--$ abcd
```
The implicit conversion to a c style string makes working with meta strings at run time or in a constexpr functions trivial.

You can perform compile time string comparisons:
```c++
constexpr int cmp = meta::compare(meta::stom_v<"less">, meta::stom_v<"more">);

std::cout << cmp << std::endl;  

//--$ -1
```
And you can perform compile time string concatenation: 
```c++
constexpr meta::string cat = instance + meta::stom_v<"!">;

std::cout << cat << std::endl;  

//--$ abcd!
```

### Custom classes 

You can use metastring to make templates accepting string literals like meta::stom_t does by using the meta::wrap 
and meta::unwrap helpers:
```c++
template <meta::wrap Str>
struct my_class {
    // my_class holds the meta::string type
    using type = meta::unwrap_t<Str>;

    // my_class holds a meta::string instance.
    static constexpr meta::string value = meta::unwrap_v<Str>;
};
```
You can now pass string literals to your class:
```c++
using custom_t = my_class<"some str">;

//--$ custom_t::type = meta::string<'s', 'o', 'm', 'e', ' ', 's', 't', 'r'>

constexpr custom_t custom;

std::cout << custom.value << std::endl;  

//--$ some str
```
You can even use meta strings instead of string literals as template arguments:
```c++
constexpr auto custom_2 = my_class<instance>{};

std::cout << custom_2.value << std::endl;  //--$ abcd
```

