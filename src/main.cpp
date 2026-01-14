#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <rt/math.hpp>
#include <rt/camera3d.hpp>
#include <rt/scene3d.hpp>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#ifndef RT_SHADER_DIR
#define RT_SHADER_DIR "src/shaders"
#endif

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

namespace {

// A linked GL program that owns its handle and caches uniform locations, so the
// render loop never has to query them by name.
class shader {
public:
    shader() = default;
    explicit shader(GLuint id) : id_(id) {}
    shader(const shader&) = delete;
    shader& operator=(const shader&) = delete;
    shader(shader&& other) noexcept
        : id_(std::exchange(other.id_, 0)), cache_(std::move(other.cache_)) {
    }
    shader& operator=(shader&& other) noexcept {
        if (this != &other) {
            reset();
            id_ = std::exchange(other.id_, 0);
            cache_ = std::move(other.cache_);
        }
        return *this;
    }
    ~shader() {
        reset();
    }
public:
    explicit operator bool() const noexcept {
        return id_ != 0;
    }
    void use() const {
        glUseProgram(id_);
    }
    void set(const char* name, const glm::mat4& value) {
        glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
    }
    void set(const char* name, const glm::mat3& value) {
        glUniformMatrix3fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
    }
    void set(const char* name, const glm::vec3& value) {
        glUniform3fv(location(name), 1, glm::value_ptr(value));
    }
private:
    void reset() {
        if (id_) {
            glDeleteProgram(id_);
            id_ = 0;
        }
        cache_.clear();
    }
    GLint location(const char* name) {
        auto it = cache_.find(name);
        if (it != cache_.end()) {
            return it->second;
        }
        GLint loc = glGetUniformLocation(id_, name);
        if (loc < 0) {
            std::cerr << "Unknown uniform: " << name << std::endl;
        }
        cache_.emplace(name, loc);
        return loc;
    }
private:
    GLuint id_ = 0;
    std::unordered_map<std::string, GLint> cache_;
};

// A VAO with its buffers, drawn either as an array or with an index buffer.
class mesh {
public:
    mesh() = default;
    mesh(const mesh&) = delete;
    mesh& operator=(const mesh&) = delete;
    mesh(mesh&& other) noexcept
        : vao_(std::exchange(other.vao_, 0))
        , vbo_(std::exchange(other.vbo_, 0))
        , ebo_(std::exchange(other.ebo_, 0))
        , count_(std::exchange(other.count_, 0))
        , mode_(other.mode_) {
    }
    mesh& operator=(mesh&& other) noexcept {
        if (this != &other) {
            reset();
            vao_   = std::exchange(other.vao_, 0);
            vbo_   = std::exchange(other.vbo_, 0);
            ebo_   = std::exchange(other.ebo_, 0);
            count_ = std::exchange(other.count_, 0);
            mode_  = other.mode_;
        }
        return *this;
    }
    ~mesh() {
        reset();
    }
public:
    void draw() const {
        glBindVertexArray(vao_);
        if (ebo_) {
            glDrawElements(mode_, count_, GL_UNSIGNED_INT, nullptr);
        } else {
            glDrawArrays(mode_, 0, count_);
        }
    }
public:
    // position + colour vertices, drawn as GL_LINES.
    static mesh from_lines(const std::vector<rt::node3d>& vertices) {
        mesh m;
        m.mode_  = GL_LINES;
        m.count_ = static_cast<GLsizei>(vertices.size());

        glGenVertexArrays(1, &m.vao_);
        glGenBuffers(1, &m.vbo_);
        glBindVertexArray(m.vao_);
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(rt::node3d), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(rt::node3d), (void*)offsetof(rt::node3d, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(rt::node3d), (void*)offsetof(rt::node3d, color));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        return m;
    }
    // position + normal vertices with an index buffer, drawn as GL_TRIANGLES.
    static mesh from_indexed(const rt::mesh3d& source) {
        mesh m;
        m.mode_  = GL_TRIANGLES;
        m.count_ = static_cast<GLsizei>(source.ind.size());

        glGenVertexArrays(1, &m.vao_);
        glGenBuffers(1, &m.vbo_);
        glGenBuffers(1, &m.ebo_);
        glBindVertexArray(m.vao_);
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo_);
        glBufferData(GL_ARRAY_BUFFER, source.vrt.size() * sizeof(rt::cube3d), source.vrt.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(rt::cube3d), (void*)offsetof(rt::cube3d, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(rt::cube3d), (void*)offsetof(rt::cube3d, normal));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, source.ind.size() * sizeof(unsigned int), source.ind.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        return m;
    }
private:
    void reset() {
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (ebo_) glDeleteBuffers(1, &ebo_);
        if (vao_) glDeleteVertexArrays(1, &vao_);
        vao_ = vbo_ = ebo_ = 0;
        count_ = 0;
    }
private:
    GLuint  vao_   = 0;
    GLuint  vbo_   = 0;
    GLuint  ebo_   = 0;
    GLsizei count_ = 0;
    GLenum  mode_  = GL_TRIANGLES;
};

} // namespace

class graphic {
private:
    static rt::camera3d camera;
    static bool mouse_pressed;
    static double last_x;
    static double last_y;
    static std::string exe_path;
private:
    static void error_callback(int code, const char* description) {
        std::cerr << "GLFW error " << code << ": " << description << std::endl;
    }
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
    // Shaders must be findable no matter where the binary is launched from, so
    // prefer the copy next to the executable and fall back to the source tree.
    static std::filesystem::path shader_dir() {
        namespace fs = std::filesystem;
        std::error_code ec;
        if (!exe_path.empty()) {
            fs::path beside = fs::absolute(exe_path, ec).parent_path() / "shaders";
            if (!ec && fs::is_directory(beside, ec)) {
                return beside;
            }
        }
        return fs::path(RT_SHADER_DIR);
    }
    static std::optional<std::string> load_shader(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Shader file not found: " << path << std::endl;
            return std::nullopt;
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
    static GLuint compile_shader(GLenum type, const std::string& src, const std::filesystem::path& path) {
        GLuint id = glCreateShader(type);
        const char* csrc = src.c_str();
        glShaderSource(id, 1, &csrc, nullptr);
        glCompileShader(id);
        int success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(id, 512, nullptr, log);
            std::cerr << "Shader error in " << path << ": " << log << std::endl;
            glDeleteShader(id);
            return 0;
        }
        return id;
    }
    static shader create_program(const std::string& vs_name, const std::string& fs_name) {
        auto dir = shader_dir();
        auto vs_path = dir / vs_name;
        auto fs_path = dir / fs_name;

        auto vs_src = load_shader(vs_path);
        auto fs_src = load_shader(fs_path);
        if (!vs_src || !fs_src) {
            return shader();
        }

        GLuint vs = compile_shader(GL_VERTEX_SHADER  , *vs_src, vs_path);
        GLuint fs = compile_shader(GL_FRAGMENT_SHADER, *fs_src, fs_path);
        if (!vs || !fs) {
            if (vs) glDeleteShader(vs);
            if (fs) glDeleteShader(fs);
            return shader();
        }

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);
        glDetachShader(prog, vs);
        glDetachShader(prog, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);

        int success;
        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(prog, 512, nullptr, log);
            std::cerr << "Program link error (" << vs_name << " + " << fs_name << "): " << log << std::endl;
            glDeleteProgram(prog);
            return shader();
        }
        return shader(prog);
    }
private:
    // Owns every GL object, so they are released while the context is still current.
    static int render(GLFWwindow* window) {
        shader line_shader = create_program("line.vert", "line.frag");
        shader cube_shader = create_program("cube.vert", "cube.frag");
        if (!line_shader || !cube_shader) {
            std::cerr << "Shader programs unavailable (looked in " << shader_dir() << "), aborting" << std::endl;
            return 1;
        }

        mesh axes = mesh::from_lines(rt::scene3d::make_axes());
        mesh grid = mesh::from_lines(rt::scene3d::make_grid());
        mesh bbox = mesh::from_lines(rt::scene3d::make_bbox());
        mesh cube = mesh::from_indexed(rt::scene3d::make_cube());

        const glm::vec3 light_pos   { 3.0f, 3.0f, 3.0f };
        const glm::vec3 light_color { 1.0f, 1.0f, 1.0f };
        const glm::vec3 cube_color  { 0.8f, 0.3f, 0.3f };
        const glm::mat4 model       { 1.0f };
        const glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));

        while (!glfwWindowShouldClose(window)) {
            process_input(window);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            float aspect = (height > 0) ? static_cast<float>(width) / height : 1.0f;

            glm::mat4 proj = rt::perspective(45.0f, aspect, 0.1f, 100.0f);
            glm::mat4 view = camera.get_view_mat();
            glm::mat4 mvp  = proj * view * model;

            // Lines are open geometry: culling them would drop half of every cross-hair.
            glDisable(GL_CULL_FACE);
            line_shader.use();
            line_shader.set("u_mvp", mvp);
            axes.draw();
            grid.draw();
            bbox.draw();

            glEnable(GL_CULL_FACE);
            cube_shader.use();
            cube_shader.set("u_mvp", mvp);
            cube_shader.set("u_model", model);
            cube_shader.set("u_normal_matrix", normal_matrix);
            cube_shader.set("u_light_pos", light_pos);
            cube_shader.set("u_light_color", light_color);
            cube_shader.set("u_object_color", cube_color);
            cube_shader.set("u_view_pos", camera.get_position());
            cube.draw();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        return 0;
    }
public:
    static bool init(const char* argv0) {
        exe_path = argv0 ? argv0 : "";
        glfwSetErrorCallback(error_callback);
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        // 4.1 core is the highest profile macOS offers, and is widely available elsewhere.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
        // Required by GLFW for core profile 3.2+ contexts on macOS.
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        return true;
    }
    static int run() {
        GLFWwindow* window = glfwCreateWindow(800, 600, "rt", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return 1;
        }
        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to load OpenGL functions" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }
        glfwSwapInterval(1);

        GLint samples = 0;
        glGetIntegerv(GL_SAMPLES, &samples);
        std::cout << "OpenGL " << glGetString(GL_VERSION)
                  << " | " << glGetString(GL_RENDERER)
                  << " | GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION)
                  << " | MSAA x" << samples << std::endl;

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);

#ifdef __APPLE__
        // Without a resize event the first frames are never presented on macOS.
        {
            int w, h;
            glfwGetWindowSize(window, &w, &h);
            glfwPollEvents();
            glfwSetWindowSize(window, w + 1, h);
            glfwSetWindowSize(window, w, h);
        }
#endif

        {
            int fb_width, fb_height;
            glfwGetFramebufferSize(window, &fb_width, &fb_height);
            glViewport(0, 0, fb_width, fb_height);
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        int status = render(window);

        glfwDestroyWindow(window);
        glfwTerminate();
        return status;
    }
};

bool graphic::mouse_pressed = false;
double graphic::last_x = 0.0;
double graphic::last_y = 0.0;
std::string graphic::exe_path;
rt::camera3d graphic::camera;

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-t") {
        console::init();
        console::run();
        return 0;
    }
    if (!graphic::init(argv[0])) {
        return 1;
    }
    return graphic::run();
}
