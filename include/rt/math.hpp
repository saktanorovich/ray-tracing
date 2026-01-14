#pragma once
#include <string>

namespace rt {
    struct point3d {
    public:
        double x;
        double y;
        double z;
    public:
        std::string to_string() const {
            return
            "(" +
                std::to_string(x) + ", " +
                std::to_string(y) + ", " +
                std::to_string(z) +
            ")";
        }
    public:
        point3d operator-(const point3d& pt) {
            return point3d {
                x - pt.x,
                y - pt.y,
                z - pt.z
            };
        }
    };

    struct vector3d {
    public:
        double x;
        double y;
        double z;
    };

    struct ray3d {
    public:
        point3d beg;
        point3d end;
    public:
        ray3d(const point3d& beg, const point3d& end)
            : beg(beg), end(end) {
        }
    };

    struct box3d {
    public:
        point3d l;
        point3d u;
    public:
        box3d(const point3d& l, const point3d& u)
            : l(l), u(u) {
        }
    };

    class math {
    private:
        static double eps;
    public:
        static void init(const double& eps) {
            eps = eps;
        }
        static point3d trace(const ray3d& ray, const box3d& box) {
            return point3d();
        }
    };
} // namespace rt