Transcoder
==========
Transcoder Interface
--------------------

.. doxygenclass:: icubaby::transcoder
   :members:
   :allow-dot-graphs:

Malformed Input
^^^^^^^^^^^^^^^
Malformed input is never an error. Instead, each ill-formed sequence is replaced by U+FFFD
REPLACEMENT CHARACTER and the transcoder's ``well_formed()`` member function returns false from then
on.

- **UTF-8**: overlong encodings, encoded surrogates, values beyond U+10FFFF, unexpected continuation
  bytes, and truncated sequences are replaced. Following the Unicode Standard's recommended practice,
  each maximal subpart of an ill-formed sequence becomes a single U+FFFD.
- **UTF-16**: each unpaired high or low surrogate is replaced.
- **UTF-32**: surrogates and values beyond U+10FFFF are replaced.

Noncharacters such as U+FFFF are valid code points and are converted like any other. U+FEFF is only
treated as a byte order mark by the :ref:`byte transcoder<Byte Transcoder Encoding Detection>`: the
other transcoders convert it like any other code point.

Call ``end_cp()`` once all of the input has been supplied. If the input ended part way through a
code point, ``end_cp()`` writes U+FFFD and ``well_formed()`` becomes false. Afterwards,
``partial()`` is false and the transcoder may be used to convert further input, although
``well_formed()`` remains false if any malformed input has been seen.

Byte Transcoder Encoding Detection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The :ref:`byte transcoder<Byte Transcoder>` (whose input type is ``std::byte``) determines the
encoding of its input from a byte order mark at the start of the stream. The byte order mark is not
copied to the output.

.. list-table::
  :header-rows: 1

  * - Initial bytes
    - Encoding
  * - EF BB BF
    - UTF-8
  * - FE FF
    - UTF-16 big-endian
  * - FF FE
    - UTF-16 little-endian
  * - 00 00 FE FF
    - UTF-32 big-endian
  * - FF FE 00 00
    - UTF-32 little-endian
  * - Anything else
    - UTF-8 (the bytes are decoded as UTF-8)

FF FE 00 00 is always read as a UTF-32 little-endian byte order mark, so UTF-16 little-endian input
whose first character is U+0000 is not recognized. Once chosen, the encoding remains in effect after
``end_cp()``.

The byte transcoder extends the standard transcoder interface with a ``selected_encoding()`` member
function which returns the encoding that it has detected.

.. doxygenenum:: icubaby::encoding

Convenience Typedefs
--------------------

.. doxygentypedef:: icubaby::tx_8
.. doxygentypedef:: icubaby::tx_16
.. doxygentypedef:: icubaby::tx_32
.. doxygentypedef:: icubaby::t8_8
.. doxygentypedef:: icubaby::t8_16
.. doxygentypedef:: icubaby::t8_32
.. doxygentypedef:: icubaby::t16_8
.. doxygentypedef:: icubaby::t16_16
.. doxygentypedef:: icubaby::t16_32
.. doxygentypedef:: icubaby::t32_8
.. doxygentypedef:: icubaby::t32_16
.. doxygentypedef:: icubaby::t32_32
