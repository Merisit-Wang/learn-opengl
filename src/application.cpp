#include <GL/glew.h>    // Needs to be placed before other gl headers
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak()

/* 反斜杠后面不能有空格 */
#define GLCall(x) do { \
    GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));\
 } while (0)

static void GLClearError()
{
    /* 循环获取错误(即清除) */
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] ("  << error << "): "
                  << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderSource {
    std::string vertex;
    std::string fragment;
};

static ShaderSource parseShader(const std::string& filePath) {
    std::fstream fstream(filePath);
    if (!fstream.is_open()) {
        std::cout << "failed open file: " << filePath << std::endl;
        return {};
    }
    enum ShaderType : int {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::stringstream ss[2];
    std::string line;
    ShaderType type = NONE;

    while (getline(fstream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = FRAGMENT;
            }
        } else {
            ss[type] << line << std::endl;
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static auto compileShader(unsigned int type, const std::string& source) {
    auto shaderId = glCreateShader(type);
    auto str = source.c_str();
    GLCall(glShaderSource(shaderId, 1, &str, NULL));
    GLCall(glCompileShader(shaderId));

    GLint result;
    GLCall(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        GLint length;
        GLCall(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(shaderId, length, &length, message));
        std::cout << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(shaderId));
        return (decltype(shaderId))0;
    }

    return shaderId;
}

static auto createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLuint program;
    GLCall(program = glCreateProgram());
    auto vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    auto fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Runs after the creation of the context
    if (glewInit() != GLEW_OK) {
        std::cout << "glew init error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    float pointers[] = {
        -0.5, -0.5, // 0
        0.5, -0.5,  // 1
        0.5, 0.5,   // 2
        -0.5, 0.5   // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint bufferId;
    GLCall(glGenBuffers(1, &bufferId));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferId));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), pointers, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    auto shaderSource = parseShader(R"(..\..\src\res\shader\basic.shader)");
    auto shader = createShader(shaderSource.vertex, shaderSource.fragment);
    GLCall(glUseProgram(shader));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}