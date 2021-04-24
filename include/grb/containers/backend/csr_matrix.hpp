
#pragma once

#include <grb/util/matrix_io.hpp>
#include <grb/containers/backend/csr_matrix_iterator.hpp>
#include <grb/containers/backend/csr_matrix_reference.hpp>

namespace grb {

template <typename T,
          typename I,
          typename Allocator>
struct csr_matrix_impl_ {
  using value_type = T;
  using index_type = I;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = csr_matrix_impl_iterator<csr_matrix_impl_>;
  using reference = csr_matrix_impl_reference<csr_matrix_impl_>;

  using tuples_type = std::vector<std::tuple<value_type, index_type, index_type>>;

  csr_matrix_impl_(index_t shape);

  csr_matrix_impl_(const import_matrix_type_<value_type, index_type>& matrix);

  void assign_tuples(const tuples_type& tuples);

  size_type m() const {
    return m_;
  }

  size_type n() const {
    return n_;
  }

  size_type nnz() const {
    return nnz_;
  }

  iterator begin() {
    return iterator(*this, 0, rowptr_[0]);
  }

  iterator end() {
    return iterator(*this, m_, rowptr_[m_]);
  }

  iterator find(index_t idx);

  void insert_tuples(tuples_type& tuples);

  size_type m_, n_;
  size_type nnz_;

  std::vector<index_type> rowptr_;
  std::vector<index_type> colind_;
  std::vector<value_type> values_;

  friend iterator;
  friend reference;
};

} // end grb

#include <grb/containers/backend/csr_matrix_impl.hpp>
