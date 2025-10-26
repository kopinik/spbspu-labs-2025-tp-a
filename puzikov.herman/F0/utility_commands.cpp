#include "utility_commands.hpp"
#include "matrix.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace
{
  puzikov::Matrix extractMatrFromPair(const std::pair< const std::string, const puzikov::Matrix > p)
  {
    return p.second;
  }
}

void puzikov::createMatrCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name;
  size_t rows, cols;
  in >> name >> rows >> cols;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  if (matrices.find(name) != matrices.end())
  {
    out << "<ALREADY CREATED>\n";
    return;
  }

  matrices[name] = Matrix { rows, cols, std::vector< double >(rows * cols) };
  out << "<SUCCESSFULLY CREATED>\n";
}

void puzikov::createFilledMatrCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name;
  size_t rows, cols;
  double number;
  in >> name >> rows >> cols >> number;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  if (matrices.find(name) != matrices.end())
  {
    out << "<ALREADY CREATED>\n";
    return;
  }

  matrices[name] = Matrix { rows, cols, std::vector< double >(rows * cols, number) };
  out << "<SUCCESSFULLY CREATED>\n";
}

void puzikov::listMatrCommand(std::ostream &out, const matrices_map &matrices)
{
  std::transform(matrices.begin(), matrices.end(), std::ostream_iterator< Matrix > { out, "\n" }, extractMatrFromPair);
}

void puzikov::sizeCommand(std::istream &in, std::ostream &out, const matrices_map &matrices)
{
  std::string name;
  in >> name;
  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto matrix_it = matrices.find(name);
  if (matrix_it == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  out << matrix_it->second.numbers.size() << '\n';
}

void puzikov::printCommand(std::istream &in, std::ostream &out, const matrices_map &matrices)
{
  std::string name;
  in >> name;
  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto matrix_it = matrices.find(name);
  if (matrix_it == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  out << matrix_it->second;
}

void puzikov::dropMatrCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name;
  in >> name;
  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto matrix_it = matrices.find(name);
  if (matrix_it == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  matrices.erase(matrix_it);
}

void puzikov::setCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name;
  size_t row, col;
  double value;
  in >> name >> row >> col >> value;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it = matrices.find(name);
  if (it == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  Matrix &m = it->second;
  if (row == 0 || col == 0 || row > m.rows_n || col > m.cols_n)
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  const size_t idx = (row - 1) * m.cols_n + (col - 1);
  m.numbers[idx] = value;
  out << "<SUCCESSFULLY UPDATED>\n";
}

void puzikov::setrowCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name;
  size_t row;
  in >> name >> row;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it = matrices.find(name);
  if (it == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  Matrix &m = it->second;
  if (row == 0 || row > m.rows_n)
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  std::vector< double > vals;
  vals.reserve(m.cols_n);
  std::copy(std::istream_iterator< double > { in }, std::istream_iterator< double > {}, std::back_inserter(vals));

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  if (vals.size() != m.cols_n)
  {
    out << "<INCORRECT NUMBER OF VALUES>\n";
    return;
  }

  const size_t base = (row - 1) * m.cols_n;
  std::copy(vals.begin(), vals.end(), m.numbers.begin() + base);
  out << "<SUCCESSFULLY UPDATED>\n";
}

void puzikov::setcolCommand(std::istream &in, std::ostream &out, matrices_map &matrices)
{
  std::string name;
  size_t col;
  in >> name >> col;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  auto it = matrices.find(name);
  if (it == matrices.end())
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  Matrix &m = it->second;
  if (col == 0 || col > m.cols_n)
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  std::vector< double > vals;
  vals.reserve(m.cols_n);
  std::copy(std::istream_iterator< double > { in }, std::istream_iterator< double > {}, std::back_inserter(vals));

  if (vals.size() != m.rows_n)
  {
    out << "<INCORRECT NUMBER OF VALUES>\n";
    return;
  }

  std::copy(vals.begin(), vals.end(), m.colwise_begin());
  out << "<SUCCESSFULLY UPDATED>\n";
}

void puzikov::setPrecisionCommand(std::istream &in, std::ostream &out)
{
  std::streamsize p;
  in >> p;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  if (p < 0 || p > 10)
  {
    out << "<INVALID COMMAND>\n";
    return;
  }

  out.precision(p);
  out << "<SUCCESSFULLY UPDATED>\n";
}

void puzikov::setEpsilonCommand(std::istream &in, std::ostream &out)
{
  double eps;
  in >> eps;

  if (!in)
  {
    out << "<INVALID INPUT>\n";
    return;
  }

  puzikov::Matrix::epsilon = eps;
  out << "<SUCCESSFULLY UPDATED>\n";
}
