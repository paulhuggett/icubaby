Transcoder Internals
====================

Byte Transcoder
---------------
.. doxygenclass:: icubaby::transcoder< std::byte, ToEncoding >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-8 to UTF-32 Transcoder
--------------------------
.. doxygenclass:: icubaby::transcoder< char8, char32_t >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-8 to UTF-16 Transcoder
--------------------------
.. doxygenclass:: icubaby::transcoder< char8, char16_t >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-8 to UTF-8 Transcoder
-------------------------
.. doxygenclass:: icubaby::transcoder< char8, char8 >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-16 to UTF-32 Transcoder
---------------------------
.. doxygenclass:: icubaby::transcoder< char16_t, char32_t >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-16 to UTF-16 Transcoder
---------------------------
.. doxygenclass:: icubaby::transcoder< char16_t, char16_t >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-16 to UTF-8 Transcoder
--------------------------
.. doxygenclass:: icubaby::transcoder< char16_t, char8 >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-32 to UTF-16 Transcoder
---------------------------
.. doxygenclass:: icubaby::transcoder< char32_t, char16_t >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-32 to UTF-8 Transcoder
--------------------------
.. doxygenclass:: icubaby::transcoder< char32_t, char8 >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-32 to UTF-32 Transcoder
---------------------------
.. doxygenclass:: icubaby::transcoder< char32_t, char32_t >
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

UTF-8 Decoder
-------------
The finite state machine used to decode UTF-8 input. It is shared by the UTF-8 to UTF-32, UTF-8 to UTF-16, and
UTF-8 to UTF-8 transcoders.

.. doxygenclass:: icubaby::details::utf8_dfa
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:

Triangulator
------------
A triangulator converts between two encodings via an intermediate UTF-32 encoding. Each of the transcoders now
converts directly between its input and output encodings so the triangulator is not used by the library itself. It is
retained as a simple reference implementation against which the unit tests check the direct transcoders.

.. doxygenclass:: icubaby::details::triangulator
  :members:
  :protected-members:
  :private-members:
  :undoc-members:
  :allow-dot-graphs:
