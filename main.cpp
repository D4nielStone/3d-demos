#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bgui.hpp>

#include <cmath>
#include <string>

static const float cubeVertices[] = {
    // positions         // colors
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
};

static GLuint cubeVAO = 0;
static GLuint cubeVBO = 0;
static GLuint cubeProgram = 0;
static float cubeColor[3] = {1.0f, 0.5f, 0.0f};

static void multiply_mat4(float out[16], const float a[16], const float b[16]) {
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int i = 0; i < 4; ++i) {
                sum += a[i * 4 + row] * b[col * 4 + i];
            }
            out[col * 4 + row] = sum;
        }
    }
}

static void identity_mat4(float m[16]) {
    for (int i = 0; i < 16; ++i) m[i] = 0.0f;
    m[0] = 1.0f;
    m[5] = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;
}

static void perspective_mat4(float m[16], float fovRadians, float aspect, float nearPlane, float farPlane) {
    float f = 1.0f / std::tan(fovRadians * 0.5f);
    for (int i = 0; i < 16; ++i) m[i] = 0.0f;
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
    m[11] = -1.0f;
    m[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
}

static void translate_mat4(float m[16], float x, float y, float z) {
    identity_mat4(m);
    m[12] = x;
    m[13] = y;
    m[14] = z;
}

static void rotate_mat4(float m[16], float angleRadians, float x, float y, float z) {
    float len = std::sqrt(x * x + y * y + z * z);
    if (len < 1e-6f) {
        identity_mat4(m);
        return;
    }
    x /= len;
    y /= len;
    z /= len;

    float c = std::cos(angleRadians);
    float s = std::sin(angleRadians);
    float t = 1.0f - c;

    m[0] = t * x * x + c;
    m[1] = t * x * y + s * z;
    m[2] = t * x * z - s * y;
    m[3] = 0.0f;

    m[4] = t * x * y - s * z;
    m[5] = t * y * y + c;
    m[6] = t * y * z + s * x;
    m[7] = 0.0f;

    m[8] = t * x * z + s * y;
    m[9] = t * y * z - s * x;
    m[10] = t * z * z + c;
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
}

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        fprintf(stderr, "Shader compile error: %s\n", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static bool create_cube_resources() {
    const char* vertexSource = R"glsl(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 uMVP;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )glsl";

    const char* fragmentSource = R"glsl(
        #version 330 core
        in vec3 vColor;
        uniform vec3 uColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(uColor, 1.0);
        }
    )glsl";

    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) return false;
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return false;
    }

    cubeProgram = glCreateProgram();
    glAttachShader(cubeProgram, vertexShader);
    glAttachShader(cubeProgram, fragmentShader);
    glLinkProgram(cubeProgram);

    GLint success = 0;
    glGetProgramiv(cubeProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(cubeProgram, sizeof(infoLog), nullptr, infoLog);
        fprintf(stderr, "Program link error: %s\n", infoLog);
        glDeleteProgram(cubeProgram);
        cubeProgram = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return cubeProgram != 0;
}

static void cleanup_cube_resources() {
    if (cubeVBO) glDeleteBuffers(1, &cubeVBO);
    if (cubeVAO) glDeleteVertexArrays(1, &cubeVAO);
    if (cubeProgram) glDeleteProgram(cubeProgram);
    cubeVBO = 0;
    cubeVAO = 0;
    cubeProgram = 0;
}

static void render_cube(float timeSeconds, int width, int height) {
    if (!cubeProgram || !cubeVAO) {
        return;
    }
    glUseProgram(cubeProgram);

    float projection[16];
    perspective_mat4(projection, 45.0f * (3.14159265f / 180.0f), float(width) / float(height), 0.1f, 100.0f);
    float view[16];
    translate_mat4(view, 0.0f, 0.0f, -3.0f);
    float model[16];
    rotate_mat4(model, timeSeconds, 1.0f, 1.0f, 0.0f);

    float pv[16];
    multiply_mat4(pv, projection, view);
    float mvp[16];
    multiply_mat4(mvp, pv, model);

    GLint mvpLocation = glGetUniformLocation(cubeProgram, "uMVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp);
    GLint colorLocation = glGetUniformLocation(cubeProgram, "uColor");
    glUniform3fv(colorLocation, 1, cubeColor);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

int main() {
    bgui::set_up();
    GLFWwindow* window = bgui::set_up_glfw(800, 600, "3DDemo");
    bgui::set_up_gl3();
    bgui::set_up_freetype();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    if (!create_cube_resources()) {
        return -1;
    }

    bgui::style_manager::get_instance().apply_theme(bgui::dark_theme());
    bgui::style_manager::get_instance().get_global().visual.background.normal->a = 0.0f;
    bgui::style_manager::get_instance().set_class("transparent", bgui::style{
        .visual = {
            .background = {
                .normal = bgui::color{0.0f, 0.0f, 0.0f, 0.8f}
            }
        }
    });
    auto& w = bgui::get_layout().add<bgui::window>("OpenGl info");
    w.add_class("transparent");
    w.add<bgui::text>(std::string("gl version:\n ") + bgui::get_gl_version(), 0.35f);
    w.add<bgui::text>(std::string("gl vendor:\n ") + bgui::get_gl_vendor(), 0.35f);
    w.add<bgui::text>(std::string("glsl version:\n ") + bgui::get_glsl_version(), 0.35f);

    auto& colorText = w.add<bgui::text>(std::string("Cube color: ") + std::to_string(cubeColor[0]) + ", " + std::to_string(cubeColor[1]) + ", " + std::to_string(cubeColor[2]), 0.35f);
    auto updateColorText = [&](void) {
        std::string buffer = "Cube color: " + std::to_string(cubeColor[0]) + ", " + std::to_string(cubeColor[1]) + ", " + std::to_string(cubeColor[2]);
        colorText.set_buffer(buffer);
    };
    auto clamp01 = [&](float value) {
        return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
    };

    auto& rowR = w.add<bgui::linear>(bgui::orientation::horizontal);
    rowR.add<bgui::button>("R-", 0.35f, [&]() { cubeColor[0] = clamp01(cubeColor[0] - 0.05f); updateColorText(); });
    rowR.add<bgui::button>("R+", 0.35f, [&]() { cubeColor[0] = clamp01(cubeColor[0] + 0.05f); updateColorText(); });

    auto& rowG = w.add<bgui::linear>(bgui::orientation::horizontal);
    rowG.add<bgui::button>("G-", 0.35f, [&]() { cubeColor[1] = clamp01(cubeColor[1] - 0.05f); updateColorText(); });
    rowG.add<bgui::button>("G+", 0.35f, [&]() { cubeColor[1] = clamp01(cubeColor[1] + 0.05f); updateColorText(); });

    auto& rowB = w.add<bgui::linear>(bgui::orientation::horizontal);
    rowB.style.layout.size_mode = bgui::vec<2, bgui::mode>{bgui::mode::match_parent, bgui::mode::wrap_content};
    rowR.style.layout.size_mode = bgui::vec<2, bgui::mode>{bgui::mode::match_parent, bgui::mode::wrap_content};
    rowG.style.layout.size_mode = bgui::vec<2, bgui::mode>{bgui::mode::match_parent, bgui::mode::wrap_content};
    rowB.add<bgui::button>("B-", 0.35f, [&]() { cubeColor[2] = clamp01(cubeColor[2] - 0.05f); updateColorText(); });
    rowB.add<bgui::button>("B+", 0.35f, [&]() { cubeColor[2] = clamp01(cubeColor[2] + 0.05f); updateColorText(); });

    while (!glfwWindowShouldClose(window)) {
        bgui::glfw_update(bgui::get_context());
        bgui::on_update();

        int width = 800;
        int height = 600;
        glfwGetFramebufferSize(window, &width, &height);

        bgui::gl3_clear();
        render_cube(static_cast<float>(glfwGetTime()), width, height);
        bgui::gl3_render(bgui::get_draw_data());
        bgui::swap_glfw();
    }

    cleanup_cube_resources();
    bgui::shutdown_freetype();
    bgui::shutdown_gl3();
    bgui::shutdown_glfw();
    bgui::shutdown_lib();
    return 0;
}
