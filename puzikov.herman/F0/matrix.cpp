#include "matrix.hpp"
#include <algorithm>
#include <iterator>

double puzikov::Matrix::epsilon = 1e-9;

std::vector< double >::iterator puzikov::Matrix::begin()
{
  return numbers.begin();
}

std::vector< double >::iterator puzikov::Matrix::end()
{
  return numbers.end();
}

std::size_t puzikov::Matrix::idx(std::size_t r, std::size_t c) const noexcept
{
  return r * cols_n + c;
}

std::ostream &puzikov::operator<<(std::ostream &out, const Matrix &m)
{
  out << "rows: " << m.rows_n << " cols: " << m.cols_n << '\n';
  for (std::size_t r = 0; r < m.rows_n; ++r)
  {
    auto first = m.numbers.begin() + r * m.cols_n;
    auto last = first + m.cols_n;
    std::copy(first, last, std::ostream_iterator< double >(out, " "));
    out << '\n';
  }
  return out;
}

double &puzikov::Matrix::Col_ptr::operator*() const
{
  const std::size_t r = k % rows;
  const std::size_t c = k / rows;
  return base[r * cols + c];
}

double &puzikov::Matrix::Col_ptr::operator[](std::ptrdiff_t n) const
{
  const std::size_t kk = static_cast< std::size_t >(static_cast< std::ptrdiff_t >(k) + n);
  const std::size_t r = kk % rows;
  const std::size_t c = kk / rows;
  return base[r * cols + c];
}

puzikov::Matrix::Col_ptr &puzikov::Matrix::Col_ptr::operator++()
{
  ++k;
  return *this;
}

puzikov::Matrix::Col_ptr puzikov::Matrix::Col_ptr::operator++(int)
{
  Col_ptr t { *this };
  ++(*this);
  return t;
}

puzikov::Matrix::Col_ptr &puzikov::Matrix::Col_ptr::operator--()
{
  --k;
  return *this;
}

puzikov::Matrix::Col_ptr puzikov::Matrix::Col_ptr::operator--(int)
{
  Col_ptr t { *this };
  --(*this);
  return t;
}

puzikov::Matrix::Col_ptr &puzikov::Matrix::Col_ptr::operator+=(std::ptrdiff_t n)
{
  k = static_cast< std::size_t >(static_cast< std::ptrdiff_t >(k) + n);
  return *this;
}

puzikov::Matrix::Col_ptr &puzikov::Matrix::Col_ptr::operator-=(std::ptrdiff_t n)
{
  k = static_cast< std::size_t >(static_cast< std::ptrdiff_t >(k) - n);
  return *this;
}

puzikov::Matrix::Col_ptr puzikov::Matrix::Col_ptr::operator+(std::ptrdiff_t n) const
{
  Col_ptr t { *this };
  t += n;
  return t;
}

puzikov::Matrix::Col_ptr puzikov::Matrix::Col_ptr::operator-(std::ptrdiff_t n) const
{
  Col_ptr t { *this };
  t -= n;
  return t;
}

std::ptrdiff_t puzikov::Matrix::Col_ptr::operator-(const puzikov::Matrix::Col_ptr &o) const
{
  return static_cast< difference_type >(k) - static_cast< difference_type >(o.k);
}

bool puzikov::Matrix::Col_ptr::operator==(const puzikov::Matrix::Col_ptr &o) const
{
  return k == o.k;
}

bool puzikov::Matrix::Col_ptr::operator!=(const puzikov::Matrix::Col_ptr &o) const
{
  return !(*this == o);
}

bool puzikov::Matrix::Col_ptr::operator<(const puzikov::Matrix::Col_ptr &o) const
{
  return k < o.k;
}

bool puzikov::Matrix::Col_ptr::operator>(const puzikov::Matrix::Col_ptr &o) const
{
  return o < *this;
}

bool puzikov::Matrix::Col_ptr::operator<=(const puzikov::Matrix::Col_ptr &o) const
{
  return !(o < *this);
}

bool puzikov::Matrix::Col_ptr::operator>=(const puzikov::Matrix::Col_ptr &o) const
{
  return !(*this < o);
}
