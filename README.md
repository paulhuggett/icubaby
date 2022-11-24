# icubaby 

A header-only C++17 Baby Library to Immediately Convert Unicode. Fast, minimal, and easy to use for converting a sequence between any of UTF-8, UTF-16, or UTF-32. 

## API

### transcoder

~~~cpp
template <typename From, typename To>
class transcoder {
public:
  using input_type = From;
  using output_type = To;

  template <typename OutputIterator>
  OutputIterator operator() (input_type c, OutputIterator dest);

  template <typename OutputIterator>
  OutputIterator finalize (OutputIterator dest);

  bool good () const;
};
~~~

Where `From` and `To` are any of `char8_t`, `char16_t`, or `char32_t`.

It’s possible for `From` and `To` to be the same character type. This can be used to both validate and/or correct unchecked input such as data arriving at a network port.

#### Member types

| Member type | Definition |
| ---- | ----------- |
| input_type | the character type from which conversions will be performed. May be any of `char8_t`, `char16_t` or `char32_t` |
| output_type | the character type to which the transcoder will convert. May be any of `char8_t`, `char16_t` or `char32_t`|

#### Member functions

| Member function | Description |
| --------------- | ----------- |
| (constructor)   | constructs a new transcoder |
| (destructor)    | destructs a transcoder |
| operator()      |  (input_type c, OutputIt dest) | 
| finalize        | call once the entire input sequence has been fed to operator() to ensures the sequence did not end with a partial character
| good            | returns true if the input represented valid input, false otherwise |


### iterator

~~~cpp
icubaby::iterator<typename Transcoder, typename OutputIterator>
~~~

`Transcoder` should be a type which implements the `transcoder<>` interface described above; `OutputIterator` should be an output-iterator which produces values of type `Transcoder::output_type`.

#### Member types

| Member type       | Definition                 |
| ----------------- | -------------------------- |
| iterator_category | `std::output_iterator_tag` |
| value_type        | `void`                     |
| difference_type   | `std::ptrdiff_t`           |
| pointer           | `void`                     |
| reference         | `void`                     |

#### Member functions

| Member function | Description |
| ---- | ----------- |
| (constructor) | constructs a new iterator<br><small>(public member function)</small> |
| operator= | passes an individual character to the associated transcoder<br><small>(public member function)</small> |
| operator* | no-op<br><small>(public member function)</small> |
| operator++ | no-op<br><small>(public member function)</small> | 
| operator++(int) | no-op<br><small>(public member function)</small> | 

# Examples

## Convert using std::copy()

The example code below converts from UTF-8 to UTF-16 using `icubaby::t8_16` (this name is just a shortened form of `icubaby::transcoder<char8_t, char16_t>`). To convert from UTF-<i>x</i> to UTF-<i>y</i> just use t<i>x</i>\_<i>y</i> (UTF-16 to UTF-8 is `t16_8`, UTF-32 to UTF-8 is `t32_8`, and so on).

In this code we use `std::copy()` to loop over the input code units and pass them to `icubaby::iterator<>`. `iterator<>` conveniently passes each code unit to the transcoder along with the supplied output iterator (`std::back_inserter(out)` here) and returns the updated iterator.

This code continues to process and generate characters even if we see badly formed input. Once all of the characters are processed, the function will return an empty optional if any input was bad. If the input was good, the UTF-16 equivalent is returned.

~~~cpp
#include "icubaby/icubaby.hpp"
#include <string_view>

// In C++17, std::u8stringview would become
// std::basic_string_view<icubaby::char8>

std::optional<std::u16string> convert (std::u8string_view const& src) {
  std::u16string out;

  // t8_16 is the class which converts from UTF-8 to UTF-16.
  // This name is a shortned form of transcoder<char8_t, char16_T>.
  icubaby::t8_16 utf_8_to_16;
  // We could combine the next three lines, if desired.
  auto it = icubaby::iterator{&utf_8_to_16, std::back_inserter (out)};
  it = std::copy (std::begin (src), std::end (src), it);
  utf_8_to_16.finalize (it);
  if (!utf_8_to_16.good ()) {
    // The input was malformed or ended with a partial character.
    return std::nullopt;
  }
  return out;
}

~~~

## Convert using an explicit loop

~~~cpp
std::optional<std::u16string>
convert2 (std::u8string const & src) {
  // The UTF-16 code units are written to 'out' via the 'it' iterator.
  std::u16string out;
  auto it = std::back_inserter (out);
  icubaby::t8_16 utf_8_to_16;
  for (auto const c: src) {
    // Pass this UTF-8 code-unit to the transcoder.
    it = utf_8_to_16(c, it);
    if (!utf_8_to_16.good()) {
      // The input was malformed. Bail immediately.
      return std::nullopt;
    }
  }
  // Tell the converter that this it the end of the sequence.
  utf_8_to_16.finalize (it)
  // Check that we didn't end with a partial character.
  if (!utf_8_to_16.good ()) {
    return std::nullopt;
  }
  return out; // Conversion was successful.
}
~~~

