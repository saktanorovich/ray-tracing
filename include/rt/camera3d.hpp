#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace rt {

class camera3d {
public:
    glm::vec3 target { 0, 0, 0 };
    float distance = 5.0f;
    float yaw      = 0.0f;
    float pitch    = 0.0f;
public:
    glm::mat4 get_view_mat() const {
        glm::vec3 pos;
        pos.x = target.x + distance * std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw));
        pos.y = target.y + distance * std::sin(glm::radians(pitch));
        pos.z = target.z + distance * std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw));
        return glm::lookAt(pos, target, { 0, 1, 0 });
    }
    void rotate(float dx,float dy){
        yaw   += dx;
        pitch += dy;
        if (pitch > +89) pitch = +89;
        if (pitch < -89) pitch = -89;
    }

    void zoom(float delta){
        distance -= delta;
        if (distance <  1) distance =  1;
        if (distance > 50) distance = 50;
    }
};

} // namespace rt
