#include <iostream>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <map>
#include <functional>
#include <numeric>
#include <cctype>
#include <string>
#include <stdexcept>
#include <cmath>
#include "commands.h"
#include "io-utils.h"

namespace sherkunov
{

double subArea(const Point& a, const Point& b)
{
  return a.x * b.y - a.y * b.x;
}

static double subAreaOp(const Point& a, const Point& b)
{
  return subArea(a, b);
}

static bool isDigitChar(char c)
{
  return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

double areaPolygon(const Polygon& polygon)
{
  const auto& p = polygon.points;
  if (p.size() < 2)
  {
    return 0.0;
  }
  double sum = 0.0;
  for (size_t i = 0; i + 1 < p.size(); ++i)
  {
    sum += subArea(p[i], p[i + 1]);
  }
  sum += subArea(p.back(), p.front());
  return std::abs(sum) / 2.0;
}

bool isEven(const Polygon& polygon)
{
  return (polygon.points.size() % 2) == 0;
}

bool isOdd(const Polygon& polygon)
{
  return (polygon.points.size() % 2) == 1;
}

bool isNum(const Polygon& polygon, size_t n)
{
  return polygon.points.size() == n;
}

double evenAreaAccumulator(double acc, const Polygon& poly)
{
  return acc + (isEven(poly) ? areaPolygon(poly) : 0.0);
}

double oddAreaAccumulator(double acc, const Polygon& poly)
{
  return acc + (isOdd(poly) ? areaPolygon(poly) : 0.0);
}

double meanAreaAccumulator(double acc, const Polygon& poly)
{
  return acc + areaPolygon(poly);
}

double areaEven(const std::vector< Polygon >& polygons)
{
  return std::accumulate(polygons.begin(), polygons.end(), 0.0, evenAreaAccumulator);
}

double areaOdd(const std::vector< Polygon >& polygons)
{
  return std::accumulate(polygons.begin(), polygons.end(), 0.0, oddAreaAccumulator);
}

double areaNum(const std::vector< Polygon >& polygons, size_t n)
{
  double sum = 0.0;
  for (const auto& polygon : polygons)
  {
    if (isNum(polygon, n))
    {
      sum += areaPolygon(polygon);
    }
  }
  return sum;
}

double areaMean(const std::vector< Polygon >& polygons)
{
  if (polygons.empty())
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, meanAreaAccumulator);
  return sum / static_cast<double>(polygons.size());
}

void area(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
  std::string subcommand;
  in >> subcommand;
  out << std::fixed << std::setprecision(1);
  if (subcommand == "EVEN")
  {
    out << areaEven(polygons);
  }
  else if (subcommand == "ODD")
  {
    out << areaOdd(polygons);
  }
  else if (subcommand == "MEAN")
  {
    out << areaMean(polygons);
  }
  else
  {
    if (subcommand.empty() || !std::all_of(subcommand.begin(), subcommand.end(), isDigitChar))
    {
      throw std::logic_error("<WRONG SUBCOMMAND>");
    }
    size_t n = std::stoull(subcommand);
    if (n < 3)
    {
      throw std::logic_error("<WRONG SUBCOMMAND>");
    }
    out << areaNum(polygons, n);
  }
}

void max(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
  std::string what;
  in >> what;
  if (polygons.empty())
  {
    throw std::logic_error("<THERE ARE NO POLYGONS>");
  }
  if (what == "AREA")
  {
    out << std::fixed << std::setprecision(1);
    out << maxArea(polygons);
  }
  else if (what == "VERTEXES")
  {
    out << maxVertexes(polygons);
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

double maxArea(const std::vector< Polygon >& polygons)
{
  double m = 0.0;
  for (const auto& p : polygons)
  {
    m = std::max(m, areaPolygon(p));
  }
  return m;
}

size_t maxVertexes(const std::vector< Polygon >& polygons)
{
  size_t m = 0;
  for (const auto& p : polygons)
  {
    m = std::max(m, p.points.size());
  }
  return m;
}

void min(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
  std::string what;
  in >> what;
  if (polygons.empty())
  {
    throw std::logic_error("<THERE ARE NO POLYGONS>");
  }
  if (what == "AREA")
  {
    out << std::fixed << std::setprecision(1);
    out << minArea(polygons);
  }
  else if (what == "VERTEXES")
  {
    out << minVertexes(polygons);
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

double minArea(const std::vector< Polygon >& polygons)
{
  double m = std::numeric_limits<double>::infinity();
  for (const auto& p : polygons)
  {
    m = std::min(m, areaPolygon(p));
  }
  if (!std::isfinite(m))
  {
    return 0.0;
  }
  return m;
}

size_t minVertexes(const std::vector< Polygon >& polygons)
{
  if (polygons.empty())
  {
    return 0;
  }
  size_t m = std::numeric_limits<size_t>::max();
  for (const auto& p : polygons)
  {
    m = std::min(m, p.points.size());
  }
  return m;
}

void count(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
  std::string subcommand;
  in >> subcommand;
  if (subcommand == "EVEN")
  {
    out << countEven(polygons);
  }
  else if (subcommand == "ODD")
  {
    out << countOdd(polygons);
  }
  else
  {
    if (subcommand.empty() || !std::all_of(subcommand.begin(), subcommand.end(), isDigitChar))
    {
      throw std::logic_error("<WRONG SUBCOMMAND>");
    }
    size_t n = std::stoull(subcommand);
    if (n < 3)
    {
      throw std::logic_error("<WRONG SUBCOMMAND>");
    }
    out << countNum(polygons, n);
  }
}

size_t countEven(const std::vector< Polygon >& polygons)
{
  size_t c = 0;
  for (const auto& p : polygons)
  {
    if (isEven(p))
    {
      ++c;
    }
  }
  return c;
}

size_t countOdd(const std::vector< Polygon >& polygons)
{
  size_t c = 0;
  for (const auto& p : polygons)
  {
    if (isOdd(p))
    {
      ++c;
    }
  }
  return c;
}

size_t countNum(const std::vector< Polygon >& polygons, size_t n)
{
  size_t c = 0;
  for (const auto& p : polygons)
  {
    if (isNum(p, n))
    {
      ++c;
    }
  }
  return c;
}

void inframe(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
  if (polygons.empty())
  {
    throw std::logic_error("<THERE ARE NO POLYGONS>");
  }
  Point point;
  in >> point;
  int minX = std::numeric_limits< int >::max();
  int maxX = std::numeric_limits< int >::min();
  int minY = std::numeric_limits< int >::max();
  int maxY = std::numeric_limits< int >::min();
  for (const auto& polygon : polygons)
  {
    if (polygon.points.empty())
    {
      continue;
    }
    int pminx = polygon.points.front().x;
    int pmaxx = polygon.points.front().x;
    int pminy = polygon.points.front().y;
    int pmaxy = polygon.points.front().y;
    for (size_t i = 1; i < polygon.points.size(); ++i)
    {
      const Point& q = polygon.points[i];
      pminx = std::min(pminx, q.x);
      pmaxx = std::max(pmaxx, q.x);
      pminy = std::min(pminy, q.y);
      pmaxy = std::max(pmaxy, q.y);
    }
    minX = std::min(minX, pminx);
    maxX = std::max(maxX, pmaxx);
    minY = std::min(minY, pminy);
    maxY = std::max(maxY, pmaxy);
  }
  if (point.x >= minX && point.x <= maxX && point.y >= minY && point.y <= maxY)
  {
    out << "<TRUE>";
  }
  else
  {
    out << "<FALSE>";
  }
}

void rightshapes(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
  (void)in;
  size_t count = 0;
  for (const auto& poly : polygons)
  {
    const auto& pts = poly.points;
    if (pts.size() < 3)
    {
      continue;
    }
    bool hasRightAngle = false;
    for (size_t i = 0; i < pts.size(); ++i)
    {
      const Point& a = pts[(i + pts.size() - 1) % pts.size()];
      const Point& b = pts[i];
      const Point& c = pts[(i + 1) % pts.size()];
      long long ux = b.x - a.x;
      long long uy = b.y - a.y;
      long long vx = c.x - b.x;
      long long vy = c.y - b.y;
      long long dot = ux * vx + uy * vy;
      if (dot == 0)
      {
        hasRightAngle = true;
        break;
      }
    }
    if (hasRightAngle)
    {
      ++count;
    }
  }
  out << count;
}

}
