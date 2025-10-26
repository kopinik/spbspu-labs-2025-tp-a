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
#include <vector>
#include "commands.h"
#include "io-utils.h"

namespace sherkunov
{

namespace
{

bool isDigitChar(char c)
{
  return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

struct AddDouble
{
  double operator()(double a, double b) const { return a + b; }
};

struct MaxDouble
{
  double operator()(double a, double b) const { return (a < b) ? b : a; }
};

struct MinDouble
{
  double operator()(double a, double b) const { return (a < b) ? a : b; }
};

struct MaxSizeT
{
  size_t operator()(size_t a, size_t b) const { return (a < b) ? b : a; }
};

struct MinSizeT
{
  size_t operator()(size_t a, size_t b) const { return (a < b) ? a : b; }
};

double subArea(const Point& a, const Point& b)
{
  return a.x * b.y - a.y * b.x;
}

double subAreaOp(const Point& a, const Point& b)
{
  return subArea(a, b);
}

struct IsEvenPoly
{
  bool operator()(const Polygon& p) const { return (p.points.size() % 2) == 0; }
};

struct IsOddPoly
{
  bool operator()(const Polygon& p) const { return (p.points.size() % 2) == 1; }
};

struct IsNumPoly
{
  size_t n;
  bool operator()(const Polygon& p) const { return p.points.size() == n; }
};

struct AreaOfPoly
{
  double operator()(double acc, const Polygon& p) const;
};

struct AreaOfPolyIf
{
  std::function<bool(const Polygon&)> pred;
  double operator()(double acc, const Polygon& p) const
  {
    return pred(p) ? AreaOfPoly{}(acc, p) : acc;
  }
};

struct CountIf
{
  std::function<bool(const Polygon&)> pred;
  size_t operator()(size_t acc, const Polygon& p) const
  {
    return pred(p) ? acc + 1 : acc;
  }
};

struct MaxAreaAcc
{
  double operator()(double acc, const Polygon& p) const
  {
    double a = 0.0;
    AreaOfPoly{}(a, p);
    return MaxDouble{}(acc, a);
  }
};

struct MinAreaAcc
{
  double operator()(double acc, const Polygon& p) const
  {
    double a = 0.0;
    AreaOfPoly{}(a, p);
    return MinDouble{}(acc, a);
  }
};

struct ByX
{
  bool operator()(const Point& a, const Point& b) const { return a.x < b.x; }
};

struct ByY
{
  bool operator()(const Point& a, const Point& b) const { return a.y < b.y; }
};

struct MakeIndexSeq
{
  size_t n;
  std::vector< size_t > operator()() const
  {
    std::vector< size_t > idx(n);
    size_t v = 0;
    std::generate(idx.begin(), idx.end(), [&v]() { return v++; });
    return idx;
  }
};

struct RightAngleAt
{
  const std::vector<Point>* pts;

  bool operator()(size_t i) const
  {
    const auto& p = *pts;
    const size_t n = p.size();
    const Point& a = p[(i + n - 1) % n];
    const Point& b = p[i];
    const Point& c = p[(i + 1) % n];
    long long ux = static_cast< long long >(b.x) - a.x;
    long long uy = static_cast< long long >(b.y) - a.y;
    long long vx = static_cast< long long >(c.x) - b.x;
    long long vy = static_cast< long long >(c.y) - b.y;
    long long dot = ux * vx + uy * vy;
    return dot == 0;
  }
};

struct HasRightAngle
{
  bool operator()(const Polygon& poly) const
  {
    const auto& pts = poly.points;
    if (pts.size() < 3)
    {
      return false;
    }
    std::vector< size_t > idx = MakeIndexSeq{ pts.size() }();
    return std::any_of(idx.begin(), idx.end(), RightAngleAt{ &pts });
  }
};

}

double AreaOfPoly::operator()(double acc, const Polygon& polygon) const
{
  const auto& p = polygon.points;
  if (p.size() < 2)
  {
    return acc;
  }
  double sum = std::inner_product(p.begin(), p.end() - 1, p.begin() + 1, 0.0, AddDouble{}, subAreaOp);
  sum += subArea(p.back(), p.front());
  return acc + std::abs(sum) / 2.0;
}

double areaPolygon(const Polygon& polygon)
{
  double acc = 0.0;
  return AreaOfPoly{}(acc, polygon) - acc;
}

bool isEven(const Polygon& polygon)
{
  return IsEvenPoly{}(polygon);
}

bool isOdd(const Polygon& polygon)
{
  return IsOddPoly{}(polygon);
}

bool isNum(const Polygon& polygon, size_t n)
{
  return IsNumPoly{ n }(polygon);
}

double evenAreaAccumulator(double acc, const Polygon& poly)
{
  return AreaOfPolyIf{ IsEvenPoly{} }(acc, poly);
}

double oddAreaAccumulator(double acc, const Polygon& poly)
{
  return AreaOfPolyIf{ IsOddPoly{} }(acc, poly);
}

double meanAreaAccumulator(double acc, const Polygon& poly)
{
  return AreaOfPoly{}(acc, poly);
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
  return std::accumulate(polygons.begin(), polygons.end(), 0.0, AreaOfPolyIf{ IsNumPoly{ n } });
}

double areaMean(const std::vector< Polygon >& polygons)
{
  if (polygons.empty())
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, meanAreaAccumulator);
  return sum / static_cast< double >(polygons.size());
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
    const bool allDigits = !subcommand.empty() &&
      std::all_of(subcommand.begin(), subcommand.end(),
                  [](char c){ return isDigitChar(c); });
    if (!allDigits)
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
    out << std::accumulate(polygons.begin(), polygons.end(), 0.0, MaxAreaAcc{});
  }
  else if (what == "VERTEXES")
  {
    size_t m = std::accumulate(polygons.begin(), polygons.end(), size_t{0},
      [](size_t acc, const Polygon& p){ return MaxSizeT{}(acc, p.points.size()); });
    out << m;
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
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
    double start = std::numeric_limits< double >::infinity();
    double m = std::accumulate(polygons.begin(), polygons.end(), start, MinAreaAcc{});
    out << (std::isfinite(m) ? m : 0.0);
  }
  else if (what == "VERTEXES")
  {
    size_t start = std::numeric_limits< size_t >::max();
    size_t m = std::accumulate(polygons.begin(), polygons.end(), start,
      [](size_t acc, const Polygon& p){ return MinSizeT{}(acc, p.points.size()); });
    out << (m == std::numeric_limits< size_t >::max() ? 0 : m);
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

void count(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
  std::string subcommand;
  in >> subcommand;
  if (subcommand == "EVEN")
  {
    out << std::accumulate(polygons.begin(), polygons.end(), size_t{0}, CountIf{ IsEvenPoly{} });
  }
  else if (subcommand == "ODD")
  {
    out << std::accumulate(polygons.begin(), polygons.end(), size_t{0}, CountIf{ IsOddPoly{} });
  }
  else
  {
    const bool allDigits = !subcommand.empty() &&
      std::all_of(subcommand.begin(), subcommand.end(),
                  [](char c){ return isDigitChar(c); });
    if (!allDigits)
    {
      throw std::logic_error("<WRONG SUBCOMMAND>");
    }
    size_t n = std::stoull(subcommand);
    if (n < 3)
    {
      throw std::logic_error("<WRONG SUBCOMMAND>");
    }
    out << std::accumulate(polygons.begin(), polygons.end(), size_t{0}, CountIf{ IsNumPoly{ n } });
  }
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

  struct PolyBounds
  {
    int minx, maxx, miny, maxy;
  };

  struct BoundsOfPoly
  {
    PolyBounds operator()(const Polygon& poly) const
    {
      PolyBounds pb;
      if (poly.points.empty())
      {
        pb.minx = pb.maxx = pb.miny = pb.maxy = 0;
        return pb;
      }
      auto xr = std::minmax_element(poly.points.begin(), poly.points.end(), ByX{});
      auto yr = std::minmax_element(poly.points.begin(), poly.points.end(), ByY{});
      pb.minx = xr.first->x;
      pb.maxx = xr.second->x;
      pb.miny = yr.first->y;
      pb.maxy = yr.second->y;
      return pb;
    }
  };

  struct AccBounds
  {
    int* minX; int* maxX; int* minY; int* maxY;
    int operator()(int, const Polygon& p) const
    {
      PolyBounds pb = BoundsOfPoly{}(p);
      *minX = std::min(*minX, pb.minx);
      *maxX = std::max(*maxX, pb.maxx);
      *minY = std::min(*minY, pb.miny);
      *maxY = std::max(*maxY, pb.maxy);
      return 0;
    }
  };

  (void)std::accumulate(polygons.begin(), polygons.end(), 0, AccBounds{ &minX, &maxX, &minY, &maxY });

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
  struct AddIfRight
  {
    size_t operator()(size_t acc, const Polygon& p) const
    {
      return HasRightAngle{}(p) ? acc + 1 : acc;
    }
  };
  size_t cnt = std::accumulate(polygons.begin(), polygons.end(), size_t{0}, AddIfRight{});
  out << cnt;
}

}
