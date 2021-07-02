#include "utils.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>

std::string camelCaseToSnakeCase(const std::string &input_string) {
  if (input_string.empty()) return {};

  std::ostringstream ss;
  ss << static_cast<char>(std::tolower(input_string.at(0)));
  // Add underscore before each capitalized character and convert the char to
  // lower case
  std::transform(
      std::next(input_string.cbegin()), input_string.cend(), std::ostream_iterator<std::string>(ss), [](const char c) {
        return (std::isupper(c) ? std::string("_") : std::string()) + std::string(1, std::tolower(c));
      });
  return ss.str();
}

std::string underscoreToDash(const std::string &input_string) {
  auto new_string = input_string;
  std::replace_if(
      new_string.begin(), new_string.end(), [](const char c) { return c == '_'; }, '-');
  return new_string;
}
