#pragma once

#include <grb/util/index.hpp>
#include <grb/containers/matrix_entry.hpp>
#include <grb/util/matrix_io.hpp>
#include <grb/containers/backend/dia_matrix_iterator.hpp>
#include <vector>
#include <map>
#include <limits>
#include <climits>

namespace grb {

template <typename T,
          std::integral I = std::size_t,
          typename Allocator = std::allocator<T>>
class dia_matrix {
public:
  using scalar_type = T;
  using index_type = I;
  using value_type = grb::matrix_entry<T, I>;

  using key_type = grb::index<I>;
  using map_type = T;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using allocator_type = Allocator;
  using index_allocator_type = typename std::allocator_traits<allocator_type>:: template rebind_alloc<index_type>;

  using iterator = dia_matrix_iterator<T,
                                       index_type>;

  using const_iterator = dia_matrix_iterator<std::add_const_t<T>,
                                             index_type>;

  using reference = grb::matrix_ref<T, index_type>;
  using const_reference = grb::matrix_ref<std::add_const_t<T>, index_type>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  dia_matrix(grb::index<I> shape) : m_(shape[0]), n_(shape[1]) {}

  grb::index<I> shape() const noexcept {
  	return {m_, n_};
  }

  size_type size() const noexcept {
    return nnz_;
  }

  dia_matrix() = default;
  ~dia_matrix() = default;
  dia_matrix(const dia_matrix&) = default;
  dia_matrix(dia_matrix&&) = default;
  dia_matrix& operator=(const dia_matrix&) = default;
  dia_matrix& operator=(dia_matrix&&) = default;

  std::pair<iterator, bool> insert(value_type&& value) {
  	auto&& [index, v] = value;
  	auto&& [i, j] = index;
  	index_type diagonal = (difference_type(j) - i) + shape()[0] - 1;
  	index_type idx = std::min(i, j);
    index_type count = std::min(m_, n_) - std::min(i, j);

    auto iter = diagonals_.find(diagonal);
    if (iter == diagonals_.end()) {
    	index_type count = std::min(m_, n_) - std::min(i, j);
    	auto [it, b] = diagonals_.insert({diagonal,
	    	 {std::vector<T>(count, false),
	    		std::vector<bool>(count, false)}
	    	});
    	iter = it;
    }

    auto&& [vec, bvec] = iter->second;

    if (!bvec[idx]) {
    	vec[idx] = v;
    	bvec[idx] = true;
      nnz_++;
      return {true, iterator(idx, m_, n_, iter, diagonals_.end())};
    } else {
      return {false, iterator(idx, m_, n_, iter, diagonals_.end())};
    }
  }

  template <typename M>
  std::pair<bool, iterator> insert_or_assign(key_type k, M&& obj) {
  	auto&& [i, j] = k;
  	index_type diagonal = (difference_type(j) - i) + shape()[0] - 1;
  	index_type idx = std::min(i, j);
    index_type count = std::min(m_, n_) - std::min(i, j);

    auto iter = diagonals_.find(diagonal);
    if (iter == diagonals_.end()) {
    	index_type count = std::min(m_, n_) - std::min(i, j);
    	auto [it, b] = diagonals_.insert({diagonal,
	    	 {std::vector<T>(count, false),
	    		std::vector<bool>(count, false)}
	    	});
    	iter = it;
    }

    auto&& [vec, bvec] = iter->second;
    vec[idx] = std::forward<M>(obj);

    if (!bvec[idx]) {
      bvec[idx] = true;
      nnz_++;
      return {true, iterator(idx, m_, n_, iter, diagonals_.end())};
    } else {
      return {false, iterator(idx, m_, n_, iter, diagonals_.end())};
    }
  }

  iterator begin() noexcept {
  	return iterator(0, m_, n_,
  		              diagonals_.begin(),
  		              diagonals_.end());
  }

  const_iterator begin() const noexcept {
  	return const_iterator(0, m_, n_,
  		              diagonals_.begin(),
  		              diagonals_.end());
  }

  iterator end() noexcept {
  	return iterator(0, m_, n_,
  		              diagonals_.end(),
  		              diagonals_.end());
  }

  const_iterator end() const noexcept {
  	return const_iterator(0, m_, n_,
  		              diagonals_.end(),
  		              diagonals_.end());
  }

private:
	index_type m_ = 0;
	index_type n_ = 0;
	size_type nnz_ = 0;

	std::map<index_type,
	         std::pair<std::vector<T>,
	         std::vector<bool>>
	        > diagonals_;
};

} // end grb
