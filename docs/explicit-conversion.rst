Explicit Conversion
===================

Let’s try converting a single Unicode emoji character 😀 (U+1F600 GRINNING FACE) expressed as four
UTF-8 code units (0xF0, 0x9F, 0x98, 0x80) to UTF-16 (where it is the surrogate pair 0xD83D, 0xDE00).

.. code-block:: cpp
  
  std::vector<char16_t> out;
  auto it = std::back_inserter (out);
  icubaby::t8_16 t;
  for (auto cu: {0xF0, 0x9F, 0x98, 0x80}) {
    it = t (cu, it);
  }
  it = t.end_cp (it);

The ``out`` vector will contain a two UTF-16 code units 0xD83D and 0xDE00.

Disecting the explicit conversion code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Define where and how the output should be written:
    
   .. code-block:: cpp
     
     std::vector<char16_t> out;
     auto it = std::back_inserter (out);
    
   For the purposes of this example, we write the encoded output to a `std::vector<char16_t>`.
   Use the container of your choice!

2. Create the transcoder instance:

   .. code-block:: cpp
     
     icubaby::t8_16 t;

   :cpp:type:`icubaby::t8_16` is an alias to a specialization of :cpp:class:`icubaby::transcoder` which converts from UTF-8 to UTF-16.

3. Pass each code unit and the output iterator to the transcoder.

   .. code-block:: cpp
     
     for (auto cu: {0xF0, 0x9F, 0x98, 0x80}) {
       it = t (cu, it);
     }

   For each code unit, call :cpp:func:`icubaby::transcoder::operator()`.
   
4. Tell the transcoder that we’ve reached the end of the input. This ensures that the sequence didn’t end part way through a code point.

   .. code-block:: cpp
     
     it = t.end_cp (it);

   It’s only necessary to make a single call to :cpp:func:`icubaby::transcoder::end_cp()` once *all* of the input has been fed to the transcoder.
