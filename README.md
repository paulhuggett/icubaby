# icubaby (A Baby Library to Immediately Convert Unicode)

A fast, minimal, and easy to use library for converting a sequence of values between any of UTF-8, UTF-16, or UTF-32.

## API

### transcoder

~~~cpp
template <typename From, typename To>
class transcoder {
public:
  using input_type = From;
  using output_type = To;

  template <typename OutputIt>
    requires std::output_iterator<OutputIt, output_type>
  OutputIt operator() (input_type c, OutputIt dest);

  bool finalize ();
  bool good ();
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
| ---- | ----------- |
| (constructor) | constructs a new transcoder |
| (destructor) | destructs a transcoder |
| operator() |  (input_type c, OutputIt dest) | 
| finalize | |
| good | |


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

## Convert from UTF-8 to UTF-16 using an explicit loop

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
  // Check that we didn't end with a partial character.
  if (!utf_8_to_16.finalize ()) {
    return std::nullopt;
  }
  return out; // Conversion was successful.
}
~~~

## Convert from UTF-8 to UTF-16 using std::copy()

~~~cpp
std::optional<std::u16string>
convert (std::u8string const & src) {
  std::u16string out;
  icubaby::t8_16 utf_8_to_16;
  std::copy (std::begin (src), std::end (src),
             icubaby::iterator{utf_8_to_16, std::back_inserter (out)});
  if (!utf_8_to_16.finalize ()) {
    // The input was malformed or ended with a partial character.
    return std::nullopt;
  }
  return out;
}
~~~
