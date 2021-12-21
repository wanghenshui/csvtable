#include <string>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <csv2/reader.hpp>
#include <cxxopts.hpp>

#include <cstdio>
#include <cstdlib>

// for debug print
template <typename... T>
void print_if(bool y, fmt::format_string<T...> fmt, T&&... args) {
  if (y) fmt::print(fmt, args...);
}

int main(int argc, char** argv) {
  cxxopts::Options options("csvtable",
                           "A easy tool for convert csv to a ascii table, used for document");
  options.add_options()(
      "d,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false"))(
      "f,filename", "csv file", cxxopts::value<std::string>())(
      "s,string", "comma line like a,b,c", cxxopts::value<std::string>())("h,help", "Print usage");

  auto result = options.parse(argc, argv);
  if (result.count("help")) {
    fmt::print("csvtable:\n{}", options.help());
    exit(0);
  }
  bool debug = result["debug"].as<bool>();
  print_if(debug, "csvtable: debug mode is on\n");

  csv2::Reader<csv2::delimiter<','>,
               csv2::quote_character<'"'>,
               csv2::first_row_is_header<true>,
               csv2::trim_policy::trim_whitespace>
      csv;

  bool is_file = false;
  bool is_stream = false;
  std::string str;
  if (result.count("filename")) {
    str = result["filename"].as<std::string>();
    is_stream = (str == "-");
    is_file = !is_stream;
  } else if (result.count("string")) {
    str = result["string"].as<std::string>();
  }
  print_if(debug, "is_file: {} is_stream:{} str:{}\n", is_file, is_stream, str);
  if (str.empty()) {
    fmt::print("filename or result string empty\n");
    return 1;
  }

  if (is_stream) {
    fmt::print("stream as input is not implement\n");
    return 0;
  }

  try {
    if (is_file) {
      if (!csv.mmap(str)) {
        fmt::print("mmap failed, file: {}\n", str);
        return 2;
      }
    } else if (!csv.parse(str)) {
      fmt::print("parse failed, content {}\n", str);
      return 3;
    }
  } catch (std::exception& e) {
    fmt::print("csvtable: mmap/parse exeception {}\n", e.what());
    print_if(debug, "str: {}\n", str);
    return 4;
  }

  std::vector<std::string> header_result;
  std::vector<size_t> size_vector;
  const auto header = csv.header();
  for (const auto cell : header) {
    std::string value;
    cell.read_value(value);
    header_result.push_back(value);
    size_vector.push_back(value.size());
  }
  print_if(debug && !header_result.empty(), "header_result: {} size_vector {}\n", header_result, size_vector);

  std::vector<std::vector<std::string>> column;
  if (!header_result.empty()) {
    column.push_back(header_result);
  }

  auto swap_if_max = [debug](std::vector<size_t>& a, std::vector<size_t>& b) {
    if (a.size() != b.size()) {
      print_if(debug, "swap failed, size not equal {}, {}\n", a.size(), b.size());
      return;
    }
    print_if(debug, "before vector: {}, {}\n", a, b);
    for (size_t i = 0; i < a.size(); i++) {
      if (a[i] < b[i]) {
        std::swap(a[i], b[i]);
      }
    }
    print_if(debug, "after vector: {}, {}\n", a, b);
  };

  for (const auto row : csv) {
    std::vector<std::string> each;
    std::vector<size_t> row_size;
    for (const auto cell : row) {
      std::string value;
      cell.read_value(value);
      row_size.push_back(value.size());
      print_if(debug, "read_value {} size {}\n", value, value.size());
      each.push_back(value);
    }
    if (size_vector.empty()) {
        size_vector = row_size;
    }
    
    if (!each.empty()) {
      swap_if_max(size_vector, row_size);
      column.push_back(each);
    }
  }
  print_if(debug, "size vector: {}\n", size_vector);
  // print!
  // +---+---+
  // | a | b | 1 + 3 + 1 + 3 + 1
  // +---+---+
  // each value begin with a ' ', end with ' ' , the magic 3
  auto print_line = [size_vector]() {
    std::string format_string;
    for (const auto size : size_vector) {
      format_string.append("+");
      const auto bar = std::string(size + 2, '-');
      format_string.append(bar);
    }
    format_string.append("+\n");
    fmt::print(format_string);
  };
  auto print_data = [size_vector](const std::vector<std::string>& v) {
    std::string format_string;
    bool notequal = size_vector.size() != v.size();
    print_if(notequal, "size_vector and value vector size not equal, break: {} {}\n", size_vector, v);
    if (notequal) return;
    for (size_t i = 0; i < size_vector.size(); i++) {
      size_t size = size_vector[i];
      auto str = v[i];
      format_string.append("| ");
      format_string.append(str);
      format_string.append(" ");
      if (str.size() < size) {
        format_string.append(size - str.size() , ' ');
      }
    }
    format_string.append("|\n");
    fmt::print(format_string);
  };
  for (const auto& c : column) {
    print_line();
    print_data(c);
  }
  print_line();
  return 0;
}
