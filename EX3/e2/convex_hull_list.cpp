#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <sstream>
#include <cmath>

using namespace std;

struct Point {
    double x, y;
};

// Cross product to determine orientation
double cross(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Compute polygon area using Shoelace formula
double polygonArea(const list<Point>& hull) {
    if (hull.size() < 3) return 0;

    double area = 0;
    auto it = hull.begin();
    auto next = it;
    ++next;

    for (; next != hull.end(); ++it, ++next) {
        area += it->x * next->y - next->x * it->y;
    }

    // Closing edge: last to first
    const Point& last = hull.back();
    const Point& first = hull.front();
    area += last.x * first.y - first.x * last.y;

    return fabs(area) / 2.0;
}

int main() {

    int n;
    cin >> n;

    if (n < 3) {
        cerr << "Error: Invalid number of points.\n";
        return 1;
    }

    list<Point> points;

    string line;
    getline(cin, line); // consume endline

    // Read points in format: x,y
    for (int i = 0; i < n; i++) {
        if (!getline(cin, line)) {
            cerr << "Error: Not enough point lines.\n";
            return 1;
        }

        replace(line.begin(), line.end(), ',', ' ');
        stringstream ss(line);
        Point p;
        if (!(ss >> p.x >> p.y)) {
            cerr << "Error: Invalid point format.\n";
            return 1;
        }
        points.push_back(p);
    }

    // Sort points lexicographically
    points.sort([](const Point& a, const Point& b) {
        return (a.x < b.x) || (a.x == b.x && a.y < b.y);
    });

    // Build convex hull using monotonic chain
    list<Point> hull;

    // Lower hull
    for (const auto& p : points) {
        while (hull.size() >= 2) {
            auto last = prev(hull.end());
            auto secondLast = prev(last);

            if (cross(*secondLast, *last, p) <= 0)
                hull.pop_back();
            else
                break;
        }
        hull.push_back(p);
    }

    // Upper hull
    size_t lowerSize = hull.size();
    for (auto it = points.rbegin(); it != points.rend(); ++it) {
        const Point& p = *it;

        while (hull.size() > lowerSize) {
            auto last = prev(hull.end());
            auto secondLast = prev(last);

            if (cross(*secondLast, *last, p) <= 0)
                hull.pop_back();
            else
                break;
        }
        hull.push_back(p);
    }

    hull.pop_back(); // last point is duplicate

    // Compute area
    double area = polygonArea(hull);

    cout << "Convex Hull Area: " << area << endl;

    return 0;
}
