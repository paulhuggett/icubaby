Utilities
=========

Character and String Types
^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygentypedef:: icubaby::char8
.. doxygentypedef:: icubaby::u8string
.. doxygentypedef:: icubaby::u8string_view

Code Point Constants
^^^^^^^^^^^^^^^^^^^^
.. doxygenvariable:: icubaby::replacement_char
.. doxygenvariable:: icubaby::zero_width_no_break_space
.. doxygenvariable:: icubaby::byte_order_mark
.. doxygenvariable:: icubaby::code_point_bits
.. doxygenvariable:: icubaby::first_high_surrogate
.. doxygenvariable:: icubaby::last_high_surrogate
.. doxygenvariable:: icubaby::first_low_surrogate
.. doxygenvariable:: icubaby::last_low_surrogate
.. doxygenvariable:: icubaby::max_code_point

Unicode Char Types
^^^^^^^^^^^^^^^^^^
.. doxygentypedef:: icubaby::character_types
.. doxygenstruct:: icubaby::is_unicode_char_type
.. doxygenvariable:: icubaby::is_unicode_char_type_v

.. doxygenstruct:: icubaby::is_unicode_input_type
.. doxygenvariable:: icubaby::is_unicode_input_v


Longest Sequence
^^^^^^^^^^^^^^^^
.. doxygenstruct:: icubaby::longest_sequence
.. doxygenvariable:: icubaby::longest_sequence_v

Index
^^^^^
Returns an iterator to the beginning of the pos’th code point in a range of code units.

    The functions documented here assume toolchain support for C++ 20 Ranges. If not available,
    an implementation with signature accepting conventional [begin, end) iterators is supplied.

.. doxygenfunction:: icubaby::index(I, S, std::size_t, Proj)
.. doxygenfunction:: icubaby::index(Range &&range, std::size_t pos, Proj proj={})

Length
^^^^^^
Returns the number of code points in a sequence of code units.

    The functions documented here assume toolchain support for C++ 20 Ranges. If not available,
    an implementation with signature accepting conventional [begin, end) iterators is supplied.

.. doxygenfunction:: icubaby::length(I first, S last, Proj proj = {})
.. doxygenfunction:: icubaby::length(Range &&range, Proj proj = {})

Surrogates
^^^^^^^^^^
Functions that determine whether a particular code point is one of the high or low surrogates.

.. doxygenfunction:: icubaby::is_high_surrogate
.. doxygenfunction:: icubaby::is_low_surrogate
.. doxygenfunction:: icubaby::is_surrogate

Code Point Start
^^^^^^^^^^^^^^^^
An overloaded function that can be used used to determine whether a code unit represents the start
of a code point.

.. doxygenfunction:: icubaby::is_code_point_start(char8)
.. doxygenfunction:: icubaby::is_code_point_start(char16_t)
.. doxygenfunction:: icubaby::is_code_point_start(char32_t)
