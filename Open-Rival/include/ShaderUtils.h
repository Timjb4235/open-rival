#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <gl/glew.h>

#include <string>

namespace Rival { namespace ShaderUtils {

    std::string readShaderSource(const char* filename);

    void printProgramLog(const GLuint program);

    void printShaderLog(const GLuint shader);

}}  // namespace Rival::ShaderUtils

#endif  // SHADER_UTILS_H
