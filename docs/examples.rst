Examples
========

View UTF-32 to UTF-16
^^^^^^^^^^^^^^^^^^^^^
An example showing conversion of an array UTF-32 encoded code points can be converted to
UTF-16 using the C++ 20 ranges interface.

.. literalinclude:: ../examples/view_utf32_to_16.cpp
    :caption: view_utf32_to_16.cpp
    :language: C++

Iterator and Algorithm
^^^^^^^^^^^^^^^^^^^^^^
The :cpp:class:`icubaby::iterator` class offers a familiar output iterator for using a
transcoder. Each code unit from the input encoding is written to the iterator and this in
turn writes the output encoding to a second iterator. This enables use of standard
algorithms such as ``std::copy()`` with the library.

.. literalinclude:: ../examples/iterator.cpp
    :caption: iterator.cpp
    :language: C++

Bytes to UTF-8
^^^^^^^^^^^^^^
This code converts an array of bytes containing the string "Hello World" in UTF-16 BE
with an initial byte order mark first to UTF-8 and then to an array of
``std::uint_least8_t``. We finally copy these values to ``std::cout``.

.. literalinclude:: ../examples/bytes_to_utf8.cpp
    :caption: bytes_to_utf8.cpp
    :language: C++

Manual Bytes to UTF-8
^^^^^^^^^^^^^^^^^^^^^
This code shows how icubaby makes it straightforward to convert a byte array to a
sequence of Unicode code units passing one byte at a time to a transcoder instance. We
take the bytes making up the string "Hello World" expressed in big endian UTF-16 (with a
byte order marker) and convert them to UTF-8 which is written directly to ``std::cout``.

.. literalinclude:: ../examples/manual_bytes_to_utf8.cpp
  :caption: manual_bytes_to_utf8.cpp
  :language: C++

