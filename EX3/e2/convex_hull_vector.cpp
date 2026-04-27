#include <iostream>
#include <fstream>
#include <vector>
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
double polygonArea(const vector<Point>& hull) {
    double area = 0;
    int n = hull.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += hull[i].x * hull[j].y - hull[j].x * hull[i].y;
    }
    return fabs(area) / 2.0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Error: Missing input file argument.\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file) {
        cerr << "Error: Cannot open file.\n";
        return 1;
    }

    int n;
    file >> n;

    if (!file || n < 3) {
        cerr << "Error: Invalid number of points.\n";
        return 1;
    }

    vector<Point> points;
    points.reserve(n);

    string line;
    getline(file, line); // consume endline

    // Read points in format: x,y
    for (int i = 0; i < n; i++) {
        if (!getline(file, line)) {
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
    sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return (a.x < b.x) || (a.x == b.x && a.y < b.y);
    });

    // Build convex hull using monotonic chain
    vector<Point> hull;

    // Lower hull
    for (const auto& p : points) {
        while (hull.size() >= 2 && cross(hull[hull.size()-2], hull.back(), p) <= 0)
            hull.pop_back();
        hull.push_back(p);
    }

    // Upper hull
    size_t lowerSize = hull.size();
    for (int i = points.size() - 1; i >= 0; i--) {
        const auto& p = points[i];
        while (hull.size() > lowerSize &&
               cross(hull[hull.size()-2], hull.back(), p) <= 0)
            hull.pop_back();
        hull.push_back(p);
    }

    hull.pop_back(); // last point is duplicate

    // Compute area
    double area = polygonArea(hull);

    cout << "Convex Hull Area: " << area << endl;

    return 0;
}
