#include <grb/grb.hpp>

#include <string>
#include <fstream>
#include <ranges>
#include <nlohmann/json.hpp>
#include <H5Cpp.h>

int main(int argc, char** argv) {
  auto m = grb::generate_random<float>({10, 10}, 0.25);

  grb::print(m, "Before Printing");

  grb::experimental::binsparse_mwrite("matrix.hdf5", m);
  auto read_m = grb::experimental::binsparse_mread<float>("matrix.hdf5");

  grb::print(read_m, "After Printing");

  return 0;
}