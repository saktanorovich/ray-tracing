#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace rt {

using vec3 = glm::vec3;
using mat4 = glm::mat4;

inline mat4 perspective(float fov, float aspect, float near, float far) {
    return glm::perspective(glm::radians(fov), aspect, near, far);
}

inline mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
    return glm::lookAt(eye, center, up);
}

struct vector3d {
public:
    double x;
    double y;
    double z;
};

struct point3d {
public:
    double x;
    double y;
    double z;
public:
    std::string to_string() const {
        return
            std::to_string(x) + " " +
            std::to_string(y) + " " +
            std::to_string(z);
    }
public:
    point3d operator-(const point3d& pt) const noexcept {
        return { x - pt.x, y - pt.y, z - pt.z };
    }
    point3d operator+(const point3d& pt) const noexcept {
        return { x + pt.x, y + pt.y, z + pt.z };
    }
};

struct ray3d {
public:
    point3d beg;
    point3d end;
public:
    ray3d(const point3d& beg, const point3d& end)
        : beg(beg), end(end) {
    }
public:
    vector3d direction() const noexcept{
        return vector3d {
            end.x - beg.x,
            end.y - beg.y,
            end.z - beg.z
        };
    }
};

struct box3d {
public:
    point3d l;
    point3d u;
public:
    box3d() :
        l{0, 0, 0},
        u{0, 0, 0}
    {
    }
    box3d(const point3d& l, const point3d& u)
        : l(l), u(u) {
    }
};

struct node3d {
    glm::vec3 position;
    glm::vec3 color;
};

struct cube3d {
    glm::vec3 position;
    glm::vec3 normal;
};

struct mesh3d {
    std::vector<cube3d> vrt;
    std::vector<unsigned int> ind;
};

class math {
private:
    inline static double eps = 1e-10;
private:
    inline static double div(double a, double b) noexcept{
        return (std::abs(b) > eps) ? (a / b) : std::numeric_limits<double>::infinity();
    }
public:
    static std::optional<point3d> trace(const ray3d& ray, const box3d& box) {
        vector3d dir = {
            ray.end.x - ray.beg.x,
            ray.end.y - ray.beg.y,
            ray.end.z - ray.beg.z
        };

        double tmin = 0.0;
        double tmax = std::numeric_limits<double>::infinity();

        double begArr[3] = {
            ray.beg.x,
            ray.beg.y,
            ray.beg.z
        };
        double dirArr[3] = {
            dir.x,
            dir.y,
            dir.z
        };
        double LArr[3] = { box.l.x, box.l.y, box.l.z };
        double UArr[3] = { box.u.x, box.u.y, box.u.z };

        for (int i = 0; i < 3; ++i) {
            if (std::abs(dirArr[i]) > eps) {
                double t1 = div(LArr[i] - begArr[i], dirArr[i]);
                double t2 = div(UArr[i] - begArr[i], dirArr[i]);
                if (t1 > t2) {
                    std::swap(t1, t2);
                }
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
            } else {
                if (begArr[i] < LArr[i] || begArr[i] > UArr[i]) {
                    return std::nullopt;
                }
            }
        }

        if (tmax >= tmin && tmax >= 0.0) {
            double t = (tmin > 0.0) ? tmin : tmax;
            return point3d {
                ray.beg.x + t * dir.x,
                ray.beg.y + t * dir.y,
                ray.beg.z + t * dir.z
            };
        }

        return std::nullopt;
    }
};

} // namespace rt
