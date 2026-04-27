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

int main() {

    vector<Point> points;
    while(true)
    {
        string input;
        getline(std::cin, input);

        if (input.rfind("Newgraph ", 0) == 0) 
        {
            int amount = 0;
            amount = stoi(input.substr(9));
            points.reserve(amount);
            for (int i = 0; i < amount; i++)
            {
                cin >> input;

                replace(input.begin(), input.end(), ',', ' ');
                stringstream ss(input);
                Point p;
                if (!(ss >> p.x >> p.y)) {
                    cerr << "Error: Invalid point format.\n";
                    return 1;
                }
                points.push_back(p);
            }
            cout << "The graph is build.\n";

        }
        if (input.rfind("CH", 0) == 0) 
        {
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
        }
        if (input.rfind("Newpoint ", 0) == 0) 
        {
            input = input.substr(9);
            replace(input.begin(), input.end(), ',', ' ');
            stringstream ss(input);
            Point p;
            if (!(ss >> p.x >> p.y)) {
                cerr << "Error: Invalid point format.\n";
                return 1;
            }
            points.push_back(p);
            cout << "The point is added.\n";
        }

        if (input.rfind("Removepoint ", 0) == 0) 
        {
            bool removed = false;
            input = input.substr(12);
            replace(input.begin(), input.end(), ',', ' ');
            stringstream ss(input);
            Point p;
            if (!(ss >> p.x >> p.y)) {
                cerr << "Error: Invalid point format.\n";
                return 1;
            }
            for (size_t i = 0; i < points.size(); i++)
            {
                if (points[i].x == p.x && points[i].y == p.y){
                    points.erase(points.begin() + i);
                    removed = true;
                }
            }
            if (removed)
                cout << "The point is removed.\n";
            else    
                cout << "The point not exist.\n";

        }








        


    }
    return 0;
}
