
#include <rt/math.hpp>
#include <iostream>

inline void init() {
    std::ios::sync_with_stdio(false);
    std::cout.precision(10);
    std::cout.setf(std::ios::fixed);
}

int main(int argc, char* argv[]) {
    init();

    // variables
    rt::point3d pointA;
    rt::point3d pointB;
    rt::point3d boxL;
    rt::point3d boxU;

    // read input
    std::cin >> pointA.x >> pointA.y >> pointA.z;
    std::cin >> pointB.x >> pointB.y >> pointB.z;
    std::cin >> boxL.x >> boxL.y >> boxL.z;
    std::cin >> boxU.x >> boxU.y >> boxU.z;

    // trace
    rt::ray3d ray(pointA, pointB);
    rt::box3d box(boxL, boxU);
    auto pt = rt::math::trace(ray, box);

    // write output
    std::cout << pt.to_string() << std::endl;
}
