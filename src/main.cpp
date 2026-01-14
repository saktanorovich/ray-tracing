#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <rt/math.hpp>
#include <rt/camera3d.hpp>
#include <rt/scene3d.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

class console {
public:
    static void init() {
        std::ios::sync_with_stdio(false);
        std::cout.precision(10);
        std::cout.setf(std::ios::fixed);
    }
    static void run() {
        rt::point3d pointA;
        rt::point3d pointB;
        rt::point3d boxL;
        rt::point3d boxU;

        std::cin >> pointA.x >> pointA.y >> pointA.z;
        std::cin >> pointB.x >> pointB.y >> pointB.z;
        std::cin >> boxL.x >> boxL.y >> boxL.z;
        std::cin >> boxU.x >> boxU.y >> boxU.z;

        rt::ray3d ray(pointA, pointB);
        rt::box3d box(boxL, boxU);
        auto pt = rt::math::trace(ray, box);

        if (pt) {
            std::cout << pt->to_string() << std::endl;
        } else {
            std::cout << "null" << std::endl;
        }
    }
};

class graphic {
private:
    static rt::camera3d camera;
    static bool mouse_pressed;
    static double last_x;
    static double last_y;
private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                mouse_pressed = true;
                glfwGetCursorPos(window, &last_x, &last_y);
            } else if (action == GLFW_RELEASE) {
                mouse_pressed = false;
            }
        }
    }
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
        if (mouse_pressed) {
            float dx = static_cast<float>(xpos - last_x);
            float dy = static_cast<float>(ypos - last_y);
            camera.rotate(+dx * 0.3f, -dy * 0.3f);
            last_x = xpos;
            last_y = ypos;
        }
    }
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        camera.zoom(static_cast<float>(yoffset));
    }
private:
    static void process_input(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }
    static std::string load_shader(const std::string& path) {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
    static GLuint compile_shader(GLenum type, const std::string& src) {
        GLuint shader = glCreateShader(type);
        const char* csrc = src.c_str();
        glShaderSource(shader, 1, &csrc, nullptr);
        glCompileShader(shader);
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::cerr << "Shader error: " << log << std::endl;
        }
        return shader;
    }
    static GLuint create_program(const std::string& vs_path, const std::string& fs_path) {
        auto vs_src = load_shader(vs_path);
        auto fs_src = load_shader(fs_path);
        GLuint vs = compile_shader(GL_VERTEX_SHADER  , vs_src);
        GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return prog;
    }
    static GLuint create_VAO(const std::vector<rt::node3d>& vertices) {
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(rt::node3d), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(rt::node3d), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(rt::node3d), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        return VAO;
    }
    static GLuint create_VAO_cube(const rt::mesh3d& mesh) {
        GLuint VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vrt.size() * sizeof(rt::cube3d), mesh.vrt.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(rt::cube3d), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(rt::cube3d), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.ind.size() * sizeof(unsigned int), mesh.ind.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        return VAO;
    }
public:
    static void init() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    static void run() {
        GLFWwindow* window = glfwCreateWindow(800, 600, "rt", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);

        glEnable(GL_DEPTH_TEST);

        GLuint line_shader = create_program("src/shaders/line.vert", "src/shaders/line.frag");
        GLuint cube_shader = create_program("src/shaders/cube.vert", "src/shaders/cube.frag");

        auto axes = rt::scene3d::make_axes();
        auto grid = rt::scene3d::make_grid();
        auto bbox = rt::scene3d::make_bbox();
        auto cube = rt::scene3d::make_cube();

        GLuint axes_VAO = create_VAO(axes);
        GLuint grid_VAO = create_VAO(grid);
        GLuint bbox_VAO = create_VAO(bbox);
        GLuint cube_VAO = create_VAO_cube(cube);

        while (!glfwWindowShouldClose(window)) {
            process_input(window);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glm::mat4 proj = rt::perspective(45.0f, (float)width / height, 0.1f, 100.0f);
            glm::mat4 view = camera.get_view_mat();
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 mvp = proj * view * model;

            glUseProgram(line_shader);
            glUniformMatrix4fv(glGetUniformLocation(line_shader, "MVP"), 1, GL_FALSE, &mvp[0][0]);
            glBindVertexArray(axes_VAO);
            glDrawArrays(GL_LINES, 0, axes.size());
            glBindVertexArray(grid_VAO);
            glDrawArrays(GL_LINES, 0, grid.size());
            glBindVertexArray(bbox_VAO);
            glDrawArrays(GL_LINES, 0, bbox.size());

            glUseProgram(cube_shader);
            glUniformMatrix4fv(glGetUniformLocation(cube_shader, "MVP"), 1, GL_FALSE, &mvp[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(cube_shader, "model"), 1, GL_FALSE, &model[0][0]);
            glUniform3f(glGetUniformLocation(cube_shader, "lightPos"), 3.0f, 3.0f, 3.0f);
            glUniform3f(glGetUniformLocation(cube_shader, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(cube_shader, "objectColor"), 0.8f, 0.3f, 0.3f);
            glBindVertexArray(cube_VAO);
            glDrawElements(GL_TRIANGLES, cube.ind.size(), GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }
};

bool graphic::mouse_pressed = false;
double graphic::last_x = 0.0;
double graphic::last_y = 0.0;
rt::camera3d graphic::camera;

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-t") {
        console::init();
        console::run();
    } else {
        graphic::init();
        graphic::run();
    }
}
