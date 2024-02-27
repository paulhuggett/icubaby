---
title: icubaby: Explicit Conversion
---
# Explicit Conversion

Let’s try converting a single Unicode emoji character 😀 (U+1F600 GRINNING FACE) expressed as four UTF-8 code units (0xF0, 0x9F, 0x98, 0x80) to UTF-16 (where it is the surrogate pair 0xD83D, 0xDE00).

~~~cpp
std::vector<char16_t> out;
auto it = std::back_inserter (out);
icubaby::t8_16 t;
for (auto cu: {0xF0, 0x9F, 0x98, 0x80}) {
  it = t (cu, it);
}
it = t.end_cp (it);
~~~

The `out` vector will contain a two UTF-16 code units 0xD83D and 0xDE00.

## Disecting this code

1.  Define where and how the output should be written:

    ~~~cpp
    std::vector<char16_t> out;
    auto it = std::back_inserter (out);
    ~~~

    For the purposes of this example, we write the encoded output to a `std::vector<char16_t>`. Use the container of your choice!

2.  Create the transcoder instance:

    ~~~cpp
    icubaby::t8_16 t;
    ~~~

    [`transcoder<>`](#transcoder) is a template class which requires two arguments to define the input and output encoding. You may use `char8_t` (in C++ 20, or [`icubaby::char8`](#char8) in C++ 17 and later) for UTF-8, `char16_t` for UTF-16, and `char32_t` for UTF-32. For example, `icubaby::transcoder<char16_t, char32_t>` will convert from UTF-16 to UTF-32; `icubaby::transcoder<char8_t, char16_t>` will convert from UTF-8 to UTF-16.

    There is a collection of [nine typedefs](#helper-types) to make this a little more compact. Each is named `icubaby::t_I_O` where I and O are 8, 16, or 32. For example, `icubaby::t16_32` is equivalent to `icubaby::transcoder<char16_t, char32_t>` and `icubaby::t8_16` means `icubaby::transcoder<char8_t, char16_t>`.

3.  Pass each code unit and the output iterator to the transcoder.

    ~~~cpp
    for (auto cu: {0xF0, 0x9F, 0x98, 0x80}) {
      it = t (cu, it);
    }
    ~~~

4.  Tell the transcoder that we’ve reached the end of the input. This ensures that the sequence didn’t end part way through a code point.

    ~~~cpp
    it = t.end_cp (it);
    ~~~

    It’s only necessary to make a single call to `end_cp()` once *all* of the input has been fed to the transcoder.
