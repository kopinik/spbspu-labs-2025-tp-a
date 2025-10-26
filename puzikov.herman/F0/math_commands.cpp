#include "math_commands.hpp"
#include "matrix.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <vector>

namespace
{
  using puzikov::Matrix;
  struct MulFunctor
  {
    Matrix *A;
    Matrix *B;
    std::size_t cols_res;
    std::size_t i;

    MulFunctor(Matrix &a, Matrix &b):
      A(&a),
      B(&b),
      cols_res(b.cols_n),
      i(0)
    {}

    double operator()()
    {
      const std::size_t r = i / cols_res;
      const std::size_t c = i % cols_res;

      const double *a_row = A->numbers.data() + r * A->cols_n;

      auto b_col_begin = B->colwise_begin() + static_cast< std::ptrdiff_t >(c * B->rows_n);

      double val = std::inner_product(a_row, a_row + A->cols_n, b_col_begin, 0.0);
      ++i;
      return val;
    }
  };
}

void puzikov::addCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A, name_B;
  in >> name_res >> name_A >> name_B;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A), it_B = matrices.find(name_B);
  if (it_A == matrices.end() || it_B == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  size_t cols_A = it_A->second.cols_n, cols_B = it_B->second.cols_n;
  size_t rows_A = it_A->second.rows_n, rows_B = it_B->second.rows_n;
  if (cols_A != cols_B || rows_A != rows_B)
  {
    out << "<SIZE MISMATCH>\n";
    return;
  }

  auto plus_op = std::bind(std::plus< double > {}, std::placeholders::_1, std::placeholders::_2);

  Matrix res { rows_A, cols_A, std::vector< double >(cols_A * rows_A) };
  std::transform(it_A->second.begin(), it_A->second.end(), it_B->second.begin(), res.numbers.begin(), plus_op);

  matrices[name_res] = std::move(res);
  out << "<SUCCESSFULLY ADDED>\n";
}

void puzikov::subCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A, name_B;
  in >> name_res >> name_A >> name_B;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A), it_B = matrices.find(name_B);
  if (it_A == matrices.end() || it_B == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  size_t cols_A = it_A->second.cols_n, cols_B = it_B->second.cols_n;
  size_t rows_A = it_A->second.rows_n, rows_B = it_B->second.rows_n;
  if (cols_A != cols_B || rows_A != rows_B)
  {
    out << "<SIZE MISMATCH>\n";
    return;
  }

  auto minus_op = std::bind(std::minus< double > {}, std::placeholders::_1, std::placeholders::_2);

  Matrix res { rows_A, cols_A, std::vector< double >(cols_A * rows_A) };
  std::transform(it_A->second.begin(), it_A->second.end(), it_B->second.begin(), res.numbers.begin(), minus_op);

  matrices[name_res] = std::move(res);
  out << "<SUCCESSFULLY SUBTRACTED>\n";
}

void puzikov::mulCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A, name_B;
  in >> name_res >> name_A >> name_B;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A), it_B = matrices.find(name_B);
  if (it_A == matrices.end() || it_B == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  Matrix &A = it_A->second;
  Matrix &B = it_B->second;

  if (A.cols_n != B.rows_n)
  {
    out << "<SIZE MISMATCH>\n";
    return;
  }

  Matrix res { A.rows_n, B.cols_n, std::vector< double >(A.rows_n * B.cols_n) };
  std::generate(res.numbers.begin(), res.numbers.end(), MulFunctor { A, B });
  matrices[name_res] = std::move(res);
  out << "<SUCCESSFULLY CREATED>\n";
}

void puzikov::mulScalarCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A;
  double scalar;
  in >> name_res >> name_A >> scalar;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A);
  if (it_A == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  auto scalar_mul_op = std::bind(std::multiplies< double > {}, std::placeholders::_1, scalar);
  size_t cols = it_A->second.cols_n, rows = it_A->second.rows_n;
  Matrix res { rows, cols, std::vector< double >(rows * cols) };
  std::transform(it_A->second.begin(), it_A->second.end(), res.numbers.begin(), scalar_mul_op);

  matrices[name_res] = std::move(res);
  out << "<SUCCESSFULLY MULTIPLIED>\n";
}

void puzikov::transposeCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A;
  in >> name_res >> name_A;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A);
  if (it_A == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  size_t cols = it_A->second.cols_n, rows = it_A->second.rows_n;
  Matrix res { cols, rows, std::vector< double >(rows * cols) };
  Matrix m = it_A->second;

  std::copy(m.colwise_begin(), m.colwise_end(), res.numbers.begin());
  out << "<SUCCESSFULLY TRANSPOSED>\n";
}

void puzikov::hstackCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A, name_B;
  in >> name_res >> name_A >> name_B;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A), it_B = matrices.find(name_B);
  if (it_A == matrices.end() || it_B == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  Matrix &A = it_A->second;
  Matrix &B = it_B->second;

  if (A.rows_n != B.rows_n)
  {
    out << "<SIZE MISMATCH>\n";
    return;
  }

  Matrix res { A.rows_n, A.cols_n + B.cols_n, std::vector< double >(A.rows_n * (A.cols_n + B.cols_n)) };
  auto intermediate_res_it = std::copy(A.colwise_begin(), A.colwise_end(), res.begin());
  std::copy(B.colwise_begin(), B.colwise_end(), intermediate_res_it);

  matrices[name_res] = std::move(res);
  out << "<SUCCESSFULLY HSTACKED>\n";
}

void puzikov::vstackCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A, name_B;
  in >> name_res >> name_A >> name_B;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A), it_B = matrices.find(name_B);
  if (it_A == matrices.end() || it_B == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  Matrix &A = it_A->second;
  Matrix &B = it_B->second;

  if (A.cols_n != B.cols_n)
  {
    out << "<SIZE MISMATCH>\n";
    return;
  }

  Matrix res { A.rows_n + B.rows_n, A.cols_n, std::vector< double >((A.rows_n + B.rows_n) * A.cols_n) };
  auto intermediate_res_it = std::copy(A.begin(), A.end(), res.begin());
  std::copy(B.begin(), B.end(), intermediate_res_it);

  matrices[name_res] = std::move(res);
  out << "<SUCCESSFULLY HSTACKED>\n";
}

void puzikov::flattenRowCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A;
  in >> name_res >> name_A;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A);
  if (it_A == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }
  Matrix &A = it_A->second;
  Matrix res { 1, A.rows_n * A.cols_n, std::vector< double >(A.rows_n * A.cols_n) };

  std::copy(A.begin(), A.end(), res.begin());

  matrices[name_res] = std::move(res);
  out << "<SUCCESSFULLY FLATTENED ROW>\n";
}

void puzikov::flattenColCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name_res, name_A;
  in >> name_res >> name_A;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it_A = matrices.find(name_A);
  if (it_A == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }
  Matrix &A = it_A->second;
  Matrix res { A.rows_n * A.cols_n, 1, std::vector< double >(A.rows_n * A.cols_n) };

  std::copy(A.colwise_begin(), A.colwise_end(), res.begin());

  matrices[name_res] = std::move(res);
  out << "<SUCCESSFULLY FLATTENED ROW>\n";
}
