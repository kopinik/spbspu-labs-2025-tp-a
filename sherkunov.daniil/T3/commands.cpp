#include <iostream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <map>
#include <functional>
#include <numeric>
#include "commands.h"

double sherkunov::subArea(const Point& a, const Point& b)
{
    return a.x * b.y - a.y * b.x;
}

double sherkunov::areaPolygon(const Polygon& polygon)
{
    const auto& p = polygon.points;
    double sum = std::inner_product(p.begin(), p.end() - 1, p.begin() + 1, 0.0, std::plus< double >{}, subArea);
    sum += subArea(p.back(), p.front());
    return std::abs(sum) / 2.0;
}

bool sherkunov::isEven(const Polygon& polygon)
{
    return polygon.points.size() % 2 == 0;
}

bool sherkunov::isOdd(const Polygon& polygon)
{
    return polygon.points.size() % 2 != 0;
}

bool sherkunov::isNum(const Polygon& polygon, size_t numOfVertexes)
{
    return polygon.points.size() == numOfVertexes;
}

double sherkunov::oddAreaAccumulator(double sum, const Polygon& polygon)
{
    return isOdd(polygon) ? sum + areaPolygon(polygon) : sum;
}

double sherkunov::evenAreaAccumulator(double sum, const Polygon& polygon)
{
    return isEven(polygon) ? sum + areaPolygon(polygon) : sum;
}

double sherkunov::meanAreaAccumulator(double sum, const Polygon& polygon)
{
    return sum + areaPolygon(polygon);
}

double sherkunov::numAreaAccumulator(double sum, const Polygon& polygon, size_t numOfVertexes)
{
    return isNum(polygon, numOfVertexes) ? sum + areaPolygon(polygon) : sum;
}

double sherkunov::areaEven(const std::vector< Polygon >& polygons)
{
    return std::accumulate(polygons.begin(), polygons.end(), 0.0, evenAreaAccumulator);
}

double sherkunov::areaOdd(const std::vector< Polygon >& polygons)
{
    return std::accumulate(polygons.begin(), polygons.end(), 0.0, oddAreaAccumulator);
}

double sherkunov::areaMean(const std::vector< Polygon >& polygons)
{
    if (polygons.size() == 0)
    {
        throw std::logic_error("<THERE ARE NO POLYGONS>");
    }
    return std::accumulate(polygons.begin(), polygons.end(), 0.0, meanAreaAccumulator) / polygons.size();
}

double sherkunov::areaNum(const std::vector< Polygon >& polygons, size_t numOfVertexes)
{
    using namespace std::placeholders;
    return std::accumulate(polygons.begin(), polygons.end(), 0.0, std::bind(numAreaAccumulator, _1, _2, numOfVertexes));
}

void sherkunov::area(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
    if (polygons.size() == 0)
    {
        throw std::logic_error("<THERE ARE NO POLYGONS>");
    }
    std::string subcommand;
    in >> subcommand;
    out << std::fixed << std::setprecision(1);

    std::map< std::string, std::function< double(const std::vector<Polygon>&) > > subcmds;
    subcmds["EVEN"] = areaEven;
    subcmds["ODD"] = areaOdd;
    subcmds["MEAN"] = areaMean;
    auto it = subcmds.find(subcommand);
    if (it != subcmds.end())
    {
        out << it->second(polygons);
    }
    else
    {
        size_t numOfVertexes = std::stoull(subcommand);
        if (numOfVertexes < 3)
        {
            throw std::logic_error("<WRONG SUBCOMMAND>");
        }

        out << areaNum(polygons, numOfVertexes);
    }
}

void sherkunov::max(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
    if (polygons.size() == 0)
    {
        throw std::logic_error("<THERE ARE NO POLYGONS>");
    }
    std::string subcommand;
    in >> subcommand;
    out << std::fixed << std::setprecision(1);

    std::map< std::string, std::function< double(const std::vector< Polygon >&) > > subcmds;
    subcmds["AREA"] = maxArea;
    subcmds["VERTEXES"] = maxVertexes;
    auto it = subcmds.find(subcommand);
    if (it != subcmds.end())
    {
        out << it->second(polygons);
    }
    else
    {
        throw std::logic_error("<WRONG SUBCOMMAND>");
    }
}

bool sherkunov::areaComparator(const Polygon& a, const Polygon& b)
{
    return areaPolygon(a) < areaPolygon(b);
}

double sherkunov::maxArea(const std::vector< Polygon >& polygons)
{
    return areaPolygon(*std::max_element(polygons.begin(), polygons.end(), areaComparator));
}

bool sherkunov::vertexesComparator(const Polygon& a, const Polygon& b)
{
    return a.points.size() < b.points.size();
}

size_t sherkunov::maxVertexes(const std::vector< Polygon >& polygons)
{
    return (*std::max_element(polygons.begin(), polygons.end(), vertexesComparator)).points.size();
}

void sherkunov::min(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
    if (polygons.size() == 0)
    {
        throw std::logic_error("<THERE ARE NO POLYGONS>");
    }
    std::string subcommand;
    in >> subcommand;
    out << std::fixed << std::setprecision(1);

    std::map< std::string, std::function< doubley(const std::vector< Polygon >&) > > subcmds;
    subcmds["AREA"] = maxArea;
    subcmds["VERTEXES"] = maxVertexes;
    auto it = subcmds.find(subcommand);
    if (it != subcmds.end())
    {
        out << it->second(polygons);
    }
    else
    {
        throw std::logic_error("<WRONG SUBCOMMAND>");
    }
}

double sherkunov::minArea(const std::vector< Polygon >& polygons)
{
    return areaPolygon(*std::min_element(polygons.begin(), polygons.end(), areaComparator));
}

size_t sherkunov::minVertexes(const std::vector< Polygon >& polygons)
{
    return (*std::min_element(polygons.begin(), polygons.end(), vertexesComparator)).points.size();
}

void sherkunov::count(const std::vector< Polygon >& polygons, std::istream& in, std::ostream& out)
{
    if (polygons.empty()) 
    {
        out << "0";
        return;
    }
    std::string subcommand;
    in >> subcommand;
    

    std::map< std::string, std::function< size_t(const std::vector< Polygon >&) > > subcmds;
    subcmds["EVEN"] = countEven;
    subcmds["ODD"] = countOdd;
    auto it = subcmds.find(subcommand);
    if (it != subcmds.end())
    {
        out << it->second(polygons);
    }
    else
    {
        size_t numOfVertexes = std::stoull(subcommand);
        if (numOfVertexes < 3)
        {
            throw std::logic_error("<WRONG SUBCOMMAND>");
        }

        out << countNum(polygons, numOfVertexes);
    }
}

size_t sherkunov::countEven(const std::vector< Polygon >& polygons)
{
    return std::count_if(polygons.begin(), polygons.end(), isEven);
}

size_t sherkunov::countOdd(const std::vector< Polygon >& polygons)
{
    return std::count_if(polygons.begin(), polygons.end(), isOdd);
}

size_t sherkunov::countNum(const std::vector< Polygon >& polygons, size_t numOfVertexes)
{
    using namespace std::placeholders;
    return std::count_if(polygons.begin(), polygons.end(), std::bind(isNum, _1, numOfVertexes));
}
