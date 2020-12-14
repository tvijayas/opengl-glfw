#include <iostream>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "loadpng.h"

// This example is taken from https://learnopengl.com/
// https://learnopengl.com/code_viewer.php?code=getting-started/hellowindow2
// The code originally used GLEW, I replaced it with Glad

// Compile:
// g++ example/c++/hellowindow2.cpp -Ibuild/include build/src/glad.c -lglfw -ldl

// g++ image.cpp -Iinclude src/glad.c -lglfw -ldl

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
#define WIDTH 800
#define HEIGHT 600


static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const struct
{
    float x, y ,z;
    float texCoordX,texCoordY;
} img_vertices[4] =
{
    { -1.0f, -1.0f, 0.f, 0.f, 0.f },
    {  1.0f, -1.0f, 0.f, 800.f, 0.f },
    { -1.0f,  1.0f, 0.f, 0.f, 600.f },
    {  1.0f,  1.0f, 0.f, 800.f, 600.f }
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";


static const char* img_vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vPos;\n"
"varying vec2 tex_cord;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos,1.0);\n"
"    tex_cord = vec2(0.5) - (vec2(vPos.x,vPos.y) * vec2(0.5)); \n"
"}\n";
 
static const char* img_fragment_shader_text =
"#version 110\n"
"varying vec2 tex_cord;\n"
"uniform sampler2D texture;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(texture, tex_cord);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
// The MAIN function, from here we start the application and run the game loop
int main()
{
	GLFWwindow* window;
	unsigned w,h;
	GLsizei len;
	unsigned char *buffer=0;
	GLchar buff[1000];
	GLuint texture;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location,u_tex;
    unsigned error;
    
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    glfwSetErrorCallback(error_callback);
    
    
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    glfwWindowHint(GLFW_RED_BITS,8);
    glfwWindowHint(GLFW_GREEN_BITS,8);
    glfwWindowHint(GLFW_BLUE_BITS,8);
    glfwWindowHint(GLFW_ALPHA_BITS,8);
    glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_API);
            
    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(img_vertices), img_vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &img_vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    glGetShaderInfoLog(vertex_shader,1000,&len,buff);
    std::cout << "VERTEX Log - " << buff << std::endl;
 
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &img_fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    glGetShaderInfoLog(fragment_shader,1000,&len,buff);
    std::cout << "FRAGMENT Log - " << buff << std::endl;    
 
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramInfoLog(program,1000,&len,buff);
    
    std::cout << "Shader LINK Log - " << buff << std::endl;  

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    //vcol_location = glGetAttribLocation(program, "vCol");
    vcol_location = glGetAttribLocation(program, "tex_cord");
    u_tex = glGetUniformLocation(program, "texture");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(img_vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(img_vertices[0]), (void*) (sizeof(float) * 3));
                                      
    // Define the viewport dimensions
    //glViewport(0, 0, WIDTH, HEIGHT);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);   
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(u_tex,0);
    
    error = lodepng_decode32_file(&buffer,&w,&h,"tiger.png");

    if(error) std::cout << "loadPNG:: " << lodepng_error_text(error) << std::endl;
      
    glTexImage2D(GL_TEXTURE_2D, 0,           /* target, level of detail */
                 GL_RGBA,                    /* internal format */
                 w, h, 0,                    /* width, height, border */
                 GL_RGBA, GL_UNSIGNED_BYTE,  /* external format, type */
                 buffer                      /* pixels */
                );
                std::cout << "glTexImage2D Loaded" << std::endl;
                
                  free(buffer);
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;
 
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
 
      
#ifdef CAM_ROTATE
        //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        //mat4x4_rotate_Y(m, m, (float) glfwGetTime());
        mat4x4_rotate_X(m, m, (float) glfwGetTime());
#else        
        mat4x4_rotate_X(m, m, (float) 0.0);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

        //glDrawArrays(GL_TRIANGLES, 1, 3);
#endif
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
