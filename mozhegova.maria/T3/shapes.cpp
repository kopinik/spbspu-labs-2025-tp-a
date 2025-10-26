#include "shapes.hpp"
#include <iterator>
#include <algorithm>
#include <delimiter.hpp>

std::istream & mozhegova::operator>>(std::istream & in, Point & dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  in >> DelimiterIO{ '(' };
  in >> dest.x >> DelimiterIO{ ';' };
  in >> dest.y >> DelimiterIO{ ')' };
  return in;
}

std::istream & mozhegova::operator>>(std::istream & in, Polygon & dest)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  size_t count = 0;
  if (!(in >> count) || count < 3)
  {
    in.setstate(std::ios::failbit);
    return in;
  }
  std::vector< Point > temp(count);
  std::copy_n(std::istream_iterator< Point >(in), count, temp.begin());
  if (!in)
  {
    in.setstate(std::ios::failbit);
    return in;
  }
  dest.points = std::move(temp);
  return in;
}

bool mozhegova::Point::operator==(const Point & rhs) const
{
  return x == rhs.x && y == rhs.y;
}

mozhegova::Point mozhegova::Point::operator-(const Point & rhs) const
{
  return {x - rhs.x, y - rhs.y};
}

bool mozhegova::Polygon::operator==(const Polygon & rhs) const
{
  return points == rhs.points;
}
