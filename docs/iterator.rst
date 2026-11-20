Iterator Interface
==================

The :cpp:class:`icubaby::iterator` class is an output iterator to which code units in the source encoding can be assigned. It
produces the equivalent code units in the output encoding and writes them to a second output iterator. This makes it
straightforward to use standard library algorithms such as
`std::copy() <https://en.cppreference.com/w/cpp/algorithm/copy>`_ or
`std::ranges::copy() <https://en.cppreference.com/w/cpp/algorithm/ranges/copy>`_ with the library.

Iterator Interface Example
--------------------------

.. code-block:: cpp
  
  using icubaby::char8;
  auto const in = std::array{static_cast<char8> (0xF0),
                             static_cast<char8> (0x9F),
                             static_cast<char8> (0x98),
                             static_cast<char8> (0x80)};
  std::vector<char16_t> out;
  icubaby::t8_16 t;
  auto it = icubaby::iterator{&t, std::back_inserter (out)};
  for (auto const cu: in) {
    *(it++) = cu;
  }
  it = t.end_cp (it);

This code creates an instance of :cpp:class:`icubaby::iterator` named ``it`` which holds two values: a pointer to
transcoder ``t`` and an output iterator
(`std::back_insert_iterator <https://en.cppreference.com/w/cpp/iterator/back_insert_iterator>`_ in this case).
Assigning a series of code units from the input to ``it`` fills the ``out`` vector with the equivalent code units in
the output encoding. Finally, passing ``it`` to the transcoder's ``end_cp()`` member function checks that the input
didn't end part way through a code point.

The above code snippet loops over the contents of the ``in`` array one code unit at a time. We can use
`std::ranges::copy() <https://en.cppreference.com/w/cpp/algorithm/ranges/copy>`_ to achieve the same effect:

.. code-block:: cpp
  
  using icubaby::char8;
  auto const in = std::array{static_cast<char8> (0xF0),
                             static_cast<char8> (0x9F),
                             static_cast<char8> (0x98),
                             static_cast<char8> (0x80)};
  std::vector<char16_t> out;
  icubaby::t8_16 t;
  auto it = std::ranges::copy (in, icubaby::iterator{&t, std::back_inserter (out)}).out;
  it = t.end_cp (it);

icubaby::iterator Reference
---------------------------
      
.. doxygenclass:: icubaby::iterator
   :members:

