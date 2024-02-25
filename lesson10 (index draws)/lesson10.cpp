#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ogldev_util.h"
#include "ogldev_math_3d.h"

GLuint VBO;
GLuint IBO;
GLint gTransformationLocation;

const char *pVSFileName = "shader.vs";
const char *pFSFileName = "shader.fs";

static void CombinedTransformation()
{
    static float Scale = 1.0f;

    Matrix4f Scaling(Scale, 0.0f, 0.0f, 0.0f,
                     0.0f, Scale, 0.0f, 0.0f,
                     0.0f, 0.0f, Scale, 0.0,
                     0.0f, 0.0f, 0.0f, 1.0f);

    glUniformMatrix4fv(gTransformationLocation, 1, GL_TRUE, &Scaling.m[0][0]);
}

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);

    CombinedTransformation();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

    glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glutPostRedisplay();

    glutSwapBuffers();
}

struct Vertex
{
    Vector3f pos;
    Vector3f color;

    Vertex() {}

    Vertex(float x, float y)
    {
        pos = Vector3f(x, y, 0.0f);

        float red = (float)rand() / (float)RAND_MAX;
        float green = (float)rand() / (float)RAND_MAX;
        float blue = (float)rand() / (float)RAND_MAX;
        color = Vector3f(red, green, blue);
    }
};

static void CreateVertexBuffer()
{
    Vertex Vertices[19];

    // Center
    Vertices[0] = Vertex(0.0f, 0.0);

    // Top row
    Vertices[1] = Vertex(-1.0f, 1.0f);
    Vertices[2] = Vertex(-0.75f, 1.0f);
    Vertices[3] = Vertex(-0.50f, 1.0f);
    Vertices[4] = Vertex(-0.25f, 1.0f);
    Vertices[5] = Vertex(-0.0f, 1.0f);
    Vertices[6] = Vertex(0.25f, 1.0f);
    Vertices[7] = Vertex(0.50f, 1.0f);
    Vertices[8] = Vertex(0.75f, 1.0f);
    Vertices[9] = Vertex(1.0f, 1.0f);

    // Bottom row
    Vertices[10] = Vertex(-1.0f, -1.0f);
    Vertices[11] = Vertex(-0.75f, -1.0f);
    Vertices[12] = Vertex(-0.50f, -1.0f);
    Vertices[13] = Vertex(-0.25f, -1.0f);
    Vertices[14] = Vertex(-0.0f, -1.0f);
    Vertices[15] = Vertex(0.25f, -1.0f);
    Vertices[16] = Vertex(0.50f, -1.0f);
    Vertices[17] = Vertex(0.75f, -1.0f);
    Vertices[18] = Vertex(1.0f, -1.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
    unsigned int Indices[] = {// Top triangles
                              0, 2, 1,
                              0, 3, 2,
                              0, 4, 3,
                              0, 5, 4,
                              0, 6, 5,
                              0, 7, 6,
                              0, 8, 7,
                              0, 9, 8,

                              // Bottom triangles
                              0, 10, 11,
                              0, 11, 12,
                              0, 12, 13,
                              0, 13, 14,
                              0, 14, 15,
                              0, 15, 16,
                              0, 16, 17,
                              0, 17, 18,

                              // Left triangle
                              0, 1, 10,

                              // Right triangle
                              0, 18, 9};

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char *pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0)
    {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar *p[1];
    p[0] = pShaderText;

    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    std::string vs, fs;

    if (!ReadFile(pVSFileName, vs))
    {
        exit(1);
    };

    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

    if (!ReadFile(pFSFileName, fs))
    {
        exit(1);
    };

    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    glLinkProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader programL '%s'\n", ErrorLog);
        exit(1);
    }

    gTransformationLocation = glGetUniformLocation(ShaderProgram, "gTransformation");
    if (gTransformationLocation == -1)
    {
        printf("Error getting uniform location of 'gTransformation'\n");
        exit(1);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);
}

int main(int argc, char **argv)
{
    srandom(getpid());

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    int width = 800;
    int height = 600;
    glutInitWindowSize(width, height);

    int x = 200;
    int y = 100;
    glutInitWindowPosition(x, y);
    int win = glutCreateWindow("Anri Malinka 2");
    printf("window id: %d\n", win);

    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
    glClearColor(Red, Green, Blue, Alpha);

    CreateVertexBuffer();
    CreateIndexBuffer();

    CompileShaders();

    glutDisplayFunc(RenderSceneCB);

    glutMainLoop();

    return 0;
}