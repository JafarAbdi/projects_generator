#include "utils.hpp"

#include <algorithm>
#include <ctime>
#include <iterator>
#include <numeric>
#include <sstream>

std::string camelCaseToSnakeCase(const std::string &input_string) {
  if (input_string.empty()) {
    return {};
  }

  std::ostringstream ss;
  ss << static_cast<char>(std::tolower(input_string.at(0)));
  // Add underscore before each capitalized character and convert the char to
  // lower case
  std::transform(
      std::next(input_string.cbegin()), input_string.cend(), std::ostream_iterator<std::string>(ss), [](const char c) {
        return (std::isupper(c) != 0 ? std::string("_") : std::string()) + std::string(1, std::tolower(c));
      });
  return ss.str();
}

std::string underscoreToDash(const std::string &input_string) {
  auto new_string = input_string;
  std::replace_if(
      new_string.begin(), new_string.end(), [](const char c) { return c == '_'; }, '-');
  return new_string;
}

std::size_t getCurrentYear() {
  std::time_t timer = std::time(nullptr);
  const std::tm *const current_time = std::localtime(&timer);
  return 1900 + current_time->tm_year;
}

std::string capitalizeWordsWithWhitespace(const std::string &input_string) {
  return std::accumulate(
      input_string.cbegin(), input_string.cend(), std::string(), [](std::string a, const std::string::value_type c) {
        if (a.empty()) {
          return std::string(1, std::toupper(c));
        }
        if (a.back() == ' ') {
          return std::move(a) + std::string(1, std::toupper(c));
        }
        if (std::ispunct(c) != 0) {
          return std::move(a) + ' ';
        }
        if (std::isupper(c) != 0) {
          return std::move(a) + ' ' + c;
        }
        return std::move(a) + c;
      });
}
