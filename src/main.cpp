#include <GL/glew.h>    // Needs to be placed before other gl headers
#include <GLFW/glfw3.h>

#include <iostream>

static auto compileShader(unsigned int type, const std::string& source) {
    auto shaderId = glCreateShader(type);
    auto str = source.c_str();
    glShaderSource(shaderId, 1, &str, NULL);
    glCompileShader(shaderId);

    GLint result;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(shaderId, length, &length, message);
        std::cout << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(shaderId);
        return (decltype(shaderId))0;
    }

    return shaderId;
}

static auto createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    auto program = glCreateProgram();
    auto vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    auto fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

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
        -0.5, -0.5,
         0, 0.5,
         0.5, -0.5
    };

    GLuint bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), pointers, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    std::string vertexShaderSource =
            "#version 330 core\n"
            "layout (location = 0) in vec4 position;\n"
            "void main() {\n"
            "   gl_Position = position;\n"
            "}\n";

    std::string fragmentShaderSource =
            "#version 330 core\n"
            "layout (location = 0) out vec4 color;\n"
            "void main() {\n"
            "   color = vec4(1.0, 0, 0, 1.0);\n"
            "}\n";

    auto shader = createShader(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}