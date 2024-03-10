C++ 20 Range Adaptor
====================
C++ 20 introduces the ranges library for more composable and less error-prone interaction with
iterators and containers. In icubaby, we can transform a range of bytes to a specified encoding or
convert a sequence of Unicode code units to a different encoding using a single range adaptor.

Example
-------

.. code-block:: cpp
  :linenos:
     
  #include <icubaby/icubaby.hpp>
  #include <vector>
  
  auto grinning_face() {
    auto const in = std::vector{char32_t{0x1F600}};
    auto const r = in | icubaby::views::transcode<char32_t, char16_t>;
    std::vector<char16_t> out;
    std::ranges::copy(r, std::back_inserter(out));
    return out;
  }


This code wants to convert a single Unicode code-point 😀 (U+1F600 GRINNING FACE) from UTF-32 to
UTF-16 where this code point is encoded in two code units as 0xD83D and 0xDE00.

You can experiment with a working example using `Compiler Explorer <https://godbolt.org/z/MaEEYnxG6>`_.

Disecting the Example
^^^^^^^^^^^^^^^^^^^^^

1. Define the input range:

   .. code-block:: cpp
     
     auto const in = std::vector{char32_t{0x1F600}};

  We express the input as a container with our input text consisting simply of U+1F600 GRINNING FACE
  expressed in UTF-32.

2. Create a range with a view of our container and pass it to the icubaby transcode range adaptor:

   .. code-block:: cpp
     
     auto const r = in | icubaby::views::transcode<char32_t, char16_t>;

  The first template argument for ``icubaby::views::transcode<>`` is the encoding of the input text
  (one of ``std::byte``, ``icubaby::char8``, ``char16_t``, ``char32_t``), the second argument is the
  desired encoding of the output text (``icubaby::char8``, ``char16_t``, ``char32_t``).

  We now have the range ``r`` containing the UTF-16 code-units that correspond to the original input
  text.

3. The final step is to record the values within the range ``r``. In C++ 20, this can be achieved
   with the ``std::ranges::copy()`` algorithm:

  .. code-block::
     
    std::vector<char16_t> out;
    std::ranges::copy(r, std::back_inserter(out));

  If you are using the C++ 23 ranges library, you can simplify this even further using
  ``std::ranges::to()``:

  .. code-block::
     
    auto const out = r | std::ranges::to<std::vector> ();

Namespace icubaby::ranges Reference
-----------------------------------

.. doxygennamespace:: icubaby::ranges

