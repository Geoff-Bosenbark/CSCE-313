#include <iostream>
#include <cstring>

struct Point
{
    int x, y;

    Point() : x(), y() {}
    Point(int _x, int _y) : x(_x), y(_y) {}
};

class Shape
{
public:
    int vertices;
    Point **points;

    Shape(int _vertices)
    {
        vertices = _vertices;
        points = new Point *[vertices];
    }

    ~Shape() // deallocate memory to prevent mem leaks
    {
        // iterate through to cover all points
        for (int i = 0; i < vertices; i++)
        {
            delete points[i];
        }
        // delete overarching array
        delete[] points;
    }

    void addPoints(Point pts[]) // Formal parameter for unsized array called pts
    {
        for (int i = 0; i < vertices; i++)
        {
            points[i] = new Point; // Allocate memory for each Point object
            memcpy(points[i], &pts[i], sizeof(Point)); // copy memory of pts into points
        }
    }

    double area() // Based off of shoelace formula for area of a polygon
    {
        // declare temp int for calculation
        int temp = 0;

        // Change the loop to use < vertices to avoid accessing out-of-bounds memory
        for (int i = 0; i < vertices; i++)
        {
            // (1) use arrow access operator (access member of a struct through a pointer)
            // Use % vertices to wrap around and ensure end of vertices is reached
            int lhs = points[i]->x * points[(i + 1) % vertices]->y;

            // (2) dereference and use dot access operator (access member of a struct directly)
            int rhs = (*points[(i + 1) % vertices]).x * (*points[i]).y;
            temp += (lhs - rhs);
        }
        return abs(temp) / 2.0; // Return value directly (using abs because cannot be negative area)
    }
};

int main()
{
    // 3 methods of defining struct objects
    Point tri1(0, 0);
    Point tri2 = {1, 2};
    Point tri3 = Point{2, 0};

    // adding points to tri
    Point triPts[3] = {tri1, tri2, tri3};
    Shape *tri = new Shape(3);

    // changed dot to arrow access operator (access member of a struct through a pointer)
    tri->addPoints(triPts);

    // Employed 'basic' method for defining struct objects
    Point quad1 = {0, 0};
    Point quad2 = {0, 2};
    Point quad3 = {2, 2};
    Point quad4 = {2, 0};

    // adding points to quad
    Point quadPts[4] = {quad1, quad2, quad3, quad4};
    Shape *quad = new Shape(4);

    // changed dot to arrow access operator (access member of a struct through a pointer)
    quad->addPoints(quadPts);

    // cout each (access member of a struct through a pointer)
    std::cout << "Tri area is: " << tri->area() << std::endl; 
    std::cout << "Quad area is: " << quad->area() << std::endl; 

    // deallocate memory from shapes
    delete tri;
    delete quad;

    return 0;
}
