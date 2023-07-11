#ifndef __SQUARE_H_
#define __SQUARE_H_
#include <string>

// Wall vertex shader code
auto* wallVertexShaderSrc = "#version 430 core\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec4 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   vertexColor = vec4(0.0f, 0.0f, 0.454f, 1.0);\n"
"}\0";

// Fragment shader code
auto* wallFragmentShaderSrc = "#version 430 core\n"
"out vec4 FragColor;\n"
"in vec4 vertexColor;\n"
"void main()\n"
"{\n"
"   FragColor = vertexColor;\n"
"}\n\0";


// Pacman vertex shader code
auto* pacmanVertexShaderSrc = "#version 430 core\n"
"in vec3 aPos;\n"
"out vec4 vertexColor;\n"
"uniform mat4 u_TransformationMat = mat4(1);\n"
"void main()\n"
"{\n"
"   vertexColor = vec4(1.0f, 1.0f, 0.0f, 1.0);\n"
"   gl_Position = u_TransformationMat * vec4(aPos, 1.0f);\n"
"}\0";

//Ghost vertex shader code
auto* ghostVertexShaderSrc = "#version 430 core\n"
"in vec3 aPos;\n"
"out vec4 vertexColor;\n"
"uniform mat4 u_TransformationMat = mat4(1);\n"
"void main()\n"
"{\n"
"   vertexColor = vec4(0.0f, 1.0f, 1.0f, 1.0);\n"
"   gl_Position = u_TransformationMat * vec4(aPos, 1.0f);\n"
"}\0";


// Fragment shader code
auto* entityFragmentShaderSrc = "#version 430 core\n"
"out vec4 FragColor;\n"
"in vec4 vertexColor;\n"
"void main()\n"
"{\n"
"   FragColor = vertexColor;\n"
"}\n\0";


#endif __SQUARE_H_
