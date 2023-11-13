#include "SampleApplication.h"

#include "util/shader_utils.h"

class HelloTriangleColorSample : public SampleApplication
{
  public:
    HelloTriangleColorSample(int argc, char **argv)
        : SampleApplication("HelloTriangleColor", argc, argv, ClientType::ES3_0)
    {}

    bool initialize() override
    {
        constexpr char kVS[] = R"(#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 color;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    color = aColor;
})";

        constexpr char kFS[] = R"(#version 300 es
precision mediump float;
out vec4 FragColor;
in vec3 color;
void main()
{
    FragColor = vec4(color, 1.0f);
})";

        mProgram = CompileProgram(kVS, kFS);
        if (!mProgram)
        {
            return false;
        }

        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);

        glGenBuffers(1, &mVBO);

        GLfloat vertices[] = {
            // positions         // colors
            0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
            0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f   // top
        };

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

        // Use the program object
        glUseProgram(mProgram);

        return true;
    }

    void destroy() override
    {
        glDeleteBuffers(1, &mVBO);
        glDeleteVertexArrays(1, &mVAO);
        glDeleteProgram(mProgram);
    }

    void draw() override
    {
        // Set the viewport
        glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

        // Clear the color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the triangle
        glBindVertexArray(mVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

  private:
    GLuint mProgram;
    GLuint mVAO;
    GLuint mVBO;
};

int main(int argc, char **argv)
{
    HelloTriangleColorSample app(argc, argv);
    return app.run();
}
