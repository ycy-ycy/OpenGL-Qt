#pragma once

#include <Windows.h>
#include <GL/glew.h>
#include <iostream>
#include <qimage.h>
#include <qstring.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

char* LoadFileContext(const char* url);

GLuint CompileShader(GLenum shaderType, const char* url);

GLuint CreateGPUProgram(const char* vs, const char* fs);

void LoadTextureImage(GLuint* texture, std::wstring url, GLint type, GLint level_of_detail, GLint internalFormat, GLint format);

void CreateGLBuffer(GLuint* buffer, GLenum type, GLenum usage, GLsizei size, const void* data);