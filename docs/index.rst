Welcome to icubaby!
===================
icubaby is a C++ Library to Immediately Convert Unicode. It is a portable, header-only,
dependency-free library for C++ 17 or later. It is fast, minimal, and easy to use for converting
sequences of text between any of the Unicode UTF encodings. Simple to use and exceptionally simple to
integrate into a project, it does not allocate dynamic memory and neither throws nor catches
exceptions. (Exceptions thrown by an output iterator that you supply are passed through to you.)

The library offers an API which converts to and from UTF-8, UTF-16, or UTF-32 encodings. It can also
consume a byte stream where an optional byte order mark at the start of the stream identifies the
source encoding and byte order.

Malformed input is never an error: each ill-formed sequence is replaced by U+FFFD REPLACEMENT
CHARACTER. See :ref:`Malformed Input` for details.

Installation
------------
icubaby is entirely contained within a single header file. Installation can be as simple as copying
that file (``include/icubaby/icubaby.hpp``) into your project. It has no dependencies and
self-configures to your environment.

Usage
-----
icubaby uses four different types to express the different Unicode encodings that it supports:

.. list-table::
  :header-rows: 1

  * - Type
    - Meaning
    - Input type?
    - Output type?
  * - `std::byte <https://en.cppreference.com/w/cpp/types/byte>`_
    - Encoding and byte order are determined by an optional byte order mark at the start of the
      stream (UTF-8 if there is none)
    - :octicon:`check`
    - :octicon:`x`
  * - :cpp:type:`icubaby::char8`
    - UTF-8. ``icubaby::char8`` is defined as ``char8_t`` when the native type is available and
      ``char`` otherwise
    - :octicon:`check`
    - :octicon:`check`
  * - ``char16_t``
    - Host-native endian UTF-16
    - :octicon:`check`
    - :octicon:`check`
  * - ``char32_t``
    - Host-native endian UTF-32
    - :octicon:`check`
    - :octicon:`check`

There are three ways to use the icubaby library depending on your needs:

1. C++ 20 Range Adaptor

   Use the C++ 20 range adaptor for expressive and concise code when working with ranges of
   elements. The snippet below converts from native-endian UTF-32 to native-endian UTF-16.

   .. code-block:: cpp
     
     auto const src = std::array{char32_t{0x1F600}};
     auto const r = src | icubaby::views::transcode<char32_t, char16_t>;
     std::vector<char16_t> out;
     std::ranges::copy(r, std::back_inserter(out));

   :doc:`Learn more <ranges>`

2. Iterator Interface

   Use the iterator interface for flexibility in processing sequences of text with
   iterator-based algorithms.

   .. code-block:: cpp
     
     auto const src = std::array{char32_t{0x1F600}};
     std::vector<char16_t> out;
     icubaby::t32_16 t;
     auto it = icubaby::iterator{&t, std::back_inserter (out)};
     it = std::copy (std::begin (src), std::end (src), it);
     t.end_cp (it);

   :doc:`Learn more <iterator>`

3. Explicit Conversion

   Drive the conversion one code unit at a time, providing fine-grained control over the conversion
   process.

   .. code-block:: cpp
    
    auto const src = std::array{char32_t{0x1F600}};
    std::vector<char16_t> out;
    icubaby::t32_16 t;
    auto it = std::back_inserter (out);
    for (auto cu: src) {
      it = t (cu, it);
    }
    t.end_cp (it);

   :doc:`Learn more <explicit-conversion>`

.. toctree::
   :hidden:
   :maxdepth: 1
   :caption: Contents:

   transcoder
   iterator
   defines
   utility
   explicit-conversion
   concepts
   ranges
   examples
   transcoder_internals

:ref:`genindex`
