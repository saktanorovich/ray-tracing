#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <rt/math.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace rt {

struct scene3d {
public:
    inline static std::vector<rt::node3d> make_axes() {
        return {
            { { 0, 0, 0 }, { 1, 0, 0 }}, {{ 1, 0, 0 }, { 1, 0, 0 } }, // X
            { { 0, 0, 0 }, { 0, 1, 0 }}, {{ 0, 1, 0 }, { 0, 1, 0 } }, // Y
            { { 0, 0, 0 }, { 0, 0, 1 }}, {{ 0, 0, 1 }, { 0, 0, 1 } }  // Z
        };
    }
    inline static std::vector<rt::node3d> make_grid(float size = 10, float step = 1) {
        std::vector<rt::node3d> grid;
        glm::vec3 color(0.5f);
        for (float i = -size; i <= +size; i += step) {
            grid.push_back({ { i, 0, -size }, color });
            grid.push_back({ { i, 0,  size }, color });
            grid.push_back({ { -size, 0, i }, color });
            grid.push_back({ {  size, 0, i }, color });
        }
        return grid;
    }
    inline static std::vector<rt::node3d> make_bbox(float size = 1.0f){
        float h=size/2;
        glm::vec3 c(1,1,0);
        std::vector<glm::vec3> corners = {
            { -h, -h, -h },
            {  h, -h, -h },
            {  h,  h, -h },
            { -h,  h, -h },
            { -h, -h,  h },
            {  h, -h,  h },
            {  h,  h,  h },
            { -h,  h,  h}
        };
        std::vector<node3d> lines;
        auto add = [&](int a, int b) {
            lines.push_back({ corners[a], c });
            lines.push_back({ corners[b], c });
        };
        add(0, 1);
        add(1, 2);
        add(2, 3);
        add(3, 0);
        add(4, 5);
        add(5, 6);
        add(6, 7);
        add(7, 4);
        add(0, 4);
        add(1, 5);
        add(2, 6);
        add(3, 7);
        return lines;
    }
    inline static rt::mesh3d make_cube(float size = 1.0f) {
        float h = size / 2;
        rt::mesh3d cube;
        glm::vec3 positions[8] = {
            { -h, -h, -h },
            {  h, -h, -h },
            {  h,  h, -h },
            { -h,  h, -h },
            { -h, -h,  h },
            {  h, -h,  h },
            {  h,  h,  h },
            { -h,  h,  h }
        };
        unsigned int idx[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            0, 4, 7, 7, 3, 0,
            1, 5, 6, 6, 2, 1,
            3, 2, 6, 6, 7, 3,
            0, 1, 5, 5, 4, 0
        };
        glm::vec3 normals[6] = {
            {  0,  0, -1 },
            {  0,  0,  1 },
            { -1,  0,  0 },
            {  1,  0,  0 },
            {  0,  1,  0 },
            {  0, -1,  0 }
        };
        for (int face= 0; face < 6; ++face){
            for (int i = 0; i < 6; ++i){
                unsigned int vi = idx[face * 6 + i];
                cube.vrt.push_back({positions[vi],normals[face]});
                cube.ind.push_back(face * 6 + i);
            }
        }
        return cube;
    }
};

} // namespace rt
