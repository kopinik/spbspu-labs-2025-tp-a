#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <iostream>
#include <vector>

namespace puzikov
{
  struct Matrix
  {
    std::size_t rows_n {};
    std::size_t cols_n {};
    std::vector< double > numbers;

    static double epsilon;

    std::size_t idx(std::size_t r, std::size_t c) const noexcept;
    std::vector< double >::iterator begin();
    std::vector< double >::iterator end();

    struct Col_ptr
    {
      using value_type = double;
      using difference_type = std::ptrdiff_t;
      using reference = double &;
      using pointer = double *;
      using iterator_category = std::random_access_iterator_tag;

      pointer base {};
      std::size_t rows {};
      std::size_t cols {};
      std::size_t k {};

      reference operator*() const;
      reference operator[](difference_type n) const;

      Col_ptr &operator++();
      Col_ptr operator++(int);
      Col_ptr &operator--();
      Col_ptr operator--(int);

      Col_ptr &operator+=(difference_type n);
      Col_ptr &operator-=(difference_type n);

      Col_ptr operator+(difference_type n) const;
      Col_ptr operator-(difference_type n) const;

      difference_type operator-(const Col_ptr &o) const;

      bool operator==(const Col_ptr &o) const;
      bool operator!=(const Col_ptr &o) const;
      bool operator<(const Col_ptr &o) const;
      bool operator>(const Col_ptr &o) const;
      bool operator<=(const Col_ptr &o) const;
      bool operator>=(const Col_ptr &o) const;
    };

    Col_ptr colwise_begin()
    {
      return Col_ptr { numbers.data(), rows_n, cols_n, 0 };
    }

    Col_ptr colwise_end()
    {
      return Col_ptr { numbers.data(), rows_n, cols_n, rows_n * cols_n };
    }
  };

  std::ostream &operator<<(std::ostream &in, const Matrix &m);
}
#endif
