Preprocessor Macros
===================

icubaby defines a number of preprocessor macros. They are all used as a means of discovering support
for compiler and standard library features or for simplifying their use in a backwards-compatible
fashion. In general, preprocessor macros are avoided if there is a way to implement the behavior
within the C++ language itself.

Configuration
-------------

``ICUBABY_INSIDE_NS``
  If this macro is defined before ``icubaby.hpp`` is included, the whole library is placed inside a
  namespace of that name. For example, after ``#define ICUBABY_INSIDE_NS mylib`` the transcoders are
  found in ``mylib::icubaby``. This lets a project bundle its own copy of icubaby without clashing
  with another copy.

Feature Detection
-----------------


.. doxygendefine:: ICUBABY_CXX20
.. doxygendefine:: ICUBABY_CPP_LIB_RANGES_DEFINED
.. doxygendefine:: ICUBABY_HAVE_RANGES
.. doxygendefine:: ICUBABY_CPP_CONCEPTS_DEFINED
.. doxygendefine:: ICUBABY_CPP_LIB_CONCEPTS_DEFINED
.. doxygendefine:: ICUBABY_HAVE_CONCEPTS
.. doxygendefine:: ICUBABY_REQUIRES
.. doxygendefine:: ICUBABY_CONCEPT_OUTPUT_ITERATOR
.. doxygendefine:: ICUBABY_CONCEPT_UNICODE_CHAR_TYPE
.. doxygendefine:: ICUBABY_NO_UNIQUE_ADDRESS
