//
//  Shader.hpp
//  emris
//
//  Created by peta on 2013/11/26.
//  Copyright (c) 2013 peta.okechan.net. All rights reserved.
//

#ifndef EmTetris_Shader_hpp
#define EmTetris_Shader_hpp

#include <GL/glut.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <glm/glm.hpp>

struct Variable
{
    std::string name;       // 変数名
    GLint location;         // 位置
    GLenum type;            // 型
    size_t typeSize;        // 型のサイズ
    unsigned int arraySize;    // 通常1、配列の場合その長さ
    
    Variable()
    : name(""), location(0), type(0), arraySize(0)
    {}
    
    Variable(char const * name, GLint const location, GLenum const type, GLint const arraySize)
    : name(name), location(location), type(type), arraySize(arraySize)
    {
        switch (type) {
            case GL_FLOAT: typeSize = sizeof(GLfloat); break;
            case GL_FLOAT_VEC2: typeSize = sizeof(GLfloat) * 2; break;
            case GL_FLOAT_VEC3: typeSize = sizeof(GLfloat) * 3; break;
            case GL_FLOAT_VEC4: typeSize = sizeof(GLfloat) * 4; break;
            case GL_INT: typeSize = sizeof(GLint); break;
            case GL_INT_VEC2: typeSize = sizeof(GLint) * 2; break;
            case GL_INT_VEC3: typeSize = sizeof(GLint) * 3; break;
            case GL_INT_VEC4: typeSize = sizeof(GLint) * 4; break;
            case GL_BOOL: typeSize = sizeof(GLboolean); break;
            case GL_BOOL_VEC2: typeSize = sizeof(GLboolean) * 2; break;
            case GL_BOOL_VEC3: typeSize = sizeof(GLboolean) * 3; break;
            case GL_BOOL_VEC4: typeSize = sizeof(GLboolean) * 4; break;
            case GL_FLOAT_MAT2: typeSize = sizeof(GLfloat) * 4; break;
            case GL_FLOAT_MAT3: typeSize = sizeof(GLfloat) * 9; break;
            case GL_FLOAT_MAT4: typeSize = sizeof(GLfloat) * 16; break;
            case GL_SAMPLER_2D: typeSize = sizeof(GLint); break;
            case GL_SAMPLER_CUBE: typeSize = sizeof(GLint); break;
            default:
                throw "Invalid type.";
        }
    }
};

struct UniformVariable : Variable
{
    UniformVariable()
    : Variable()
    {}
    
    UniformVariable(char const * name, GLint const location, GLenum const type, GLint const arraySize)
    : Variable(name, location, type, arraySize)
    {}
};

struct AttributeVariable : Variable
{
    AttributeVariable()
    : Variable()
    {}
    
    AttributeVariable(char const * name, GLint const location, GLenum const type, GLint const arraySize)
    : Variable(name, location, type, arraySize)
    {}
};

struct Shader
{
    std::unordered_map<std::string, UniformVariable> uniforms;
    std::unordered_map<std::string, AttributeVariable> attributes;
    GLuint program;
    
    Shader(std::string const vshCode, std::string const fshCode)
    : uniforms(), attributes(), program()
    {
        LoadShader(vshCode, fshCode);
        DetectVariables();
    }
    
    ~Shader()
    {
        glDeleteProgram(program);
    }
    
    void setUniform(std::string const & name, GLfloat const & data, GLsizei const count = 1)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform1fv(v.location, count, reinterpret_cast<GLfloat const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::vec2 const & data, GLsizei const count = 1)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform2fv(v.location, count, reinterpret_cast<GLfloat const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::vec3 const & data, GLsizei const count = 1)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform3fv(v.location, count, reinterpret_cast<GLfloat const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::vec4 const & data, GLsizei const count = 1)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform4fv(v.location, count, reinterpret_cast<GLfloat const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::mat2 const & data, GLsizei const count = 1, GLboolean transpose = false)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniformMatrix2fv(v.location, count, transpose, reinterpret_cast<GLfloat const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::mat3 const & data, GLsizei const count = 1, GLboolean transpose = false)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniformMatrix3fv(v.location, count, transpose, reinterpret_cast<GLfloat const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::mat4 const & data, GLsizei const count = 1, GLboolean transpose = false)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniformMatrix4fv(v.location, count, transpose, reinterpret_cast<GLfloat const *>(&data));
    }
    
    void setUniform(std::string const & name, GLint const & data, GLsizei const count = 1)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform1iv(v.location, count, reinterpret_cast<GLint const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::ivec2 const & data, GLsizei const count = 1)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform2iv(v.location, count, reinterpret_cast<GLint const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::ivec3 const & data, GLsizei const count = 1)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform3iv(v.location, count, reinterpret_cast<GLint const *>(&data));
    }
    
    void setUniform(std::string const & name, glm::ivec4 const & data, GLsizei const count = 1)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform4iv(v.location, count, reinterpret_cast<GLint const *>(&data));
    }
    
    void setUniform(std::string const & name, GLboolean const & data)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform1i(v.location, data);
    }
    
    void setUniform(std::string const & name, glm::bvec2 const & data)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform2i(v.location, data[0], data[1]);
    }
    
    void setUniform(std::string const & name, glm::bvec3 const & data)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform3i(v.location, data[0], data[1], data[2]);
    }
    
    void setUniform(std::string const & name, glm::bvec4 const & data)
    {
        auto & v = uniforms.at(name);
        assert(sizeof(data) >= v.typeSize);
        glUniform4i(v.location, data[0], data[1], data[2], data[3]);
    }
    
    template<typename T>
    void setUniform(std::string const & name, std::vector<T> const & data)
    {
        assert(data.size() > 0);
        setUniform(name, data[0], static_cast<GLsizei>(data.size()));
    }
    
    template<typename T>
    void getUniform(std::string const & name, T *outv)
    {
        auto & v = uniforms.at(name);
        glGetUniformfv(program, v.location, outv);
    }
    
private:
    void LoadShader(std::string const vshCode, std::string const fshCode)
    {
        GLuint vsh = 0, fsh = 0;
        
        program = glCreateProgram();
        
        if (!Compile(vsh, GL_VERTEX_SHADER, vshCode.c_str())) {
            std::cout << "Vertex shader compile error: " << vshCode << std::endl;
            return;
        }
        
        if (!Compile(fsh, GL_FRAGMENT_SHADER, fshCode.c_str())) {
            std::cout << "Fragment shader compile error: " << fshCode << std::endl;
            return;
        }
        
        glAttachShader(program, vsh);
        glAttachShader(program, fsh);
        
        if (!Link()) {
            std::cout << "Shader program link error: " << std::endl;
            
            if (vsh) glDeleteShader(vsh);
            if (fsh) glDeleteShader(fsh);
            
            if (program) {
                glDeleteProgram(program);
                program = 0;
            }
            return;
        }
        
        glDetachShader(program, vsh);
        glDetachShader(program, fsh);
        glDeleteShader(vsh);
        glDeleteShader(fsh);
    }
    
    void PrintShaderInfoLog(GLuint shader, std::string const title)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            std::cout << title << ": " << log << std::endl;
            free(log);
        }
    }
    
    void PrintProgramInfoLog(GLuint program, std::string const title)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(program, logLength, &logLength, log);
            std::cout << title << ": " << log << std::endl;
            free(log);
        }
    }
    
    bool Compile(GLuint &ret, GLenum type, char const *code)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);
        
        PrintShaderInfoLog(shader, "Shader compile log");
        
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            glDeleteShader(shader);
            ret = 0;
            return false;
        }
        
        ret = shader;
        return true;
    }
    
    bool Link()
    {
        glLinkProgram(program);
        
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            PrintProgramInfoLog(program, "Program link log");
            return false;
        }
        
        return true;
    }
    
    void DetectVariables()
    {
        GLint count;
        GLint maxLength;
        
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
        for (int i = 0; i < count; i++) {
            GLsizei const bufSize = 255;
            char name[bufSize] = {0};
            GLint size;
            GLenum type;
            GLint location;
            
            glGetActiveUniform(program, i, bufSize, NULL, &size, &type, name);
            location = glGetUniformLocation(program, name);
            uniforms[name] = std::move(UniformVariable(name, location, type, size));
        }
        
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
        for (int i = 0; i < count; i++) {
            GLsizei const bufSize = 255;
            char name[bufSize] = {0};
            GLint size;
            GLenum type;
            GLint location;
            
            glGetActiveAttrib(program, i, bufSize, NULL, &size, &type, name);
            location = glGetAttribLocation(program, name);
            attributes[name] = std::move(AttributeVariable(name, location, type, size));
        }
    }
};

#endif
