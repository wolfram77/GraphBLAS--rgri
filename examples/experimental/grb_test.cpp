#include <grb/grb.hpp>
#include <CL/sycl.hpp>
#include <span>
#include <fmt/core.h>
#include "sycl_help.hpp"

template <std::ranges::random_access_range R>
void test_random_access_range(R&&) {}

template <std::forward_iterator I>
void test_iterator(I&&) {}

int main(int argc, char** argv) {
  namespace sycl = cl::sycl;

  sycl::device d;

  try {
    d = sycl::device(sycl::cpu_selector());
    std::cout << "Running on device \"" << d.get_info<sycl::info::device::name>() << "\"" << std::endl;
  } catch (sycl::exception const &e) {
    std::cout << "Cannot select an accelerator\n" << e.what() << "\n";
    std::cout << "Using a CPU device\n";
    d = sycl::device(sycl::cpu_selector());
  }

  sycl::queue q(d);
  shared_allocator<bool> allocator(q);

  shared_matrix<bool> m("../data/chesapeake.mtx", allocator);

  for (auto&& [index, value] : m) {
    auto&& [i, j] = index;
    if (i % 2 == 0) {
      value = true;
    } else {
      value = false;
    }
  }

  using grb::detail::spanner;

  spanner m_view(m);

  grb::print(m, "Original values");

  for_each(spanner(m), [=](auto&& entry) {
                     auto&& [index, value] = entry;
                     auto&& [i, j] = index;
                     if (i == 38 && j == 7) {
                       value = 0;
                     }
                    }, q);

  grb::print(m, "After assigning");

  for (auto&& [index, value] : m) {
    auto&& [i, j] = index;
    if (i % 2 == 0 && bool(value) != true) {
      fmt::print("Error: {}, {}: {} is wrong.\n", i, j, bool(value));
    } else if (i % 2 == 1 && bool(value) == true) {
      fmt::print("Error: {}, {}: {} is wrong.\n", i, j, bool(value));
    }
  }

  shared_allocator<bool> allocator_int(q);

  shared_vector<bool> v(100, allocator_int);

  test_iterator(v.begin());

/*
  for_each(spanner(v), [=](auto&& entry) {
                         auto&& [index, value] = entry;
                         if (index == 10) {
                           value = 12;
                         }
                       });
                       */

  return 0;
}



