//
//  ObjectData.hpp
//  emris
//
//  Created by peta on 2013/12/16.
//  Copyright (c) 2013 peta.okechan.net. All rights reserved.
//

#ifndef EmTetris_Box_h
#define EmTetris_Box_h

#include <vector>
#include <unordered_map>
#include <string>
#include <GL/glut.h>
#include <glm/glm.hpp>

struct ObjectData
{
public:
    struct BoxChar
    {
        unsigned char character;
        unsigned int width;
        std::vector<glm::vec2> dotData;
    };
    using CharMap = std::unordered_map<unsigned char, BoxChar>;
    
private:
    static size_t boxVertexCount;
    static size_t bevelBoxVertexCount;
    static size_t backgroundVertexCount;
    static size_t currentObjectVertexCount;
    static CharMap charMap;
    
public:
    static void UseBox(GLuint const positionAttribId, GLuint const normalAttribId)
    {
        static GLuint vbo = 0;
        
        if (!vbo) {
            std::vector<float> vdata = {
                // 手前面
                -0.5f, +0.5f, +0.5f,    +0.0f, +0.0f, +1.0f,
                -0.5f, -0.5f, +0.5f,    +0.0f, +0.0f, +1.0f,
                +0.5f, +0.5f, +0.5f,    +0.0f, +0.0f, +1.0f,
                +0.5f, +0.5f, +0.5f,    +0.0f, +0.0f, +1.0f,
                -0.5f, -0.5f, +0.5f,    +0.0f, +0.0f, +1.0f,
                +0.5f, -0.5f, +0.5f,    +0.0f, +0.0f, +1.0f,
                
                // 後ろ面
                -0.5f, +0.5f, -0.5f,    +0.0f, +0.0f, -1.0f,
                +0.5f, +0.5f, -0.5f,    +0.0f, +0.0f, -1.0f,
                -0.5f, -0.5f, -0.5f,    +0.0f, +0.0f, -1.0f,
                -0.5f, -0.5f, -0.5f,    +0.0f, +0.0f, -1.0f,
                +0.5f, +0.5f, -0.5f,    +0.0f, +0.0f, -1.0f,
                +0.5f, -0.5f, -0.5f,    +0.0f, +0.0f, -1.0f,
                
                // 左側面
                -0.5f, +0.5f, -0.5f,    -1.0f, +0.0f, +0.0f,
                -0.5f, -0.5f, -0.5f,    -1.0f, +0.0f, +0.0f,
                -0.5f, +0.5f, +0.5f,    -1.0f, +0.0f, +0.0f,
                -0.5f, +0.5f, +0.5f,    -1.0f, +0.0f, +0.0f,
                -0.5f, -0.5f, -0.5f,    -1.0f, +0.0f, +0.0f,
                -0.5f, -0.5f, +0.5f,    -1.0f, +0.0f, +0.0f,
                
                // 右側面
                +0.5f, +0.5f, -0.5f,    +1.0f, +0.0f, +0.0f,
                +0.5f, +0.5f, +0.5f,    +1.0f, +0.0f, +0.0f,
                +0.5f, -0.5f, -0.5f,    +1.0f, +0.0f, +0.0f,
                +0.5f, -0.5f, -0.5f,    +1.0f, +0.0f, +0.0f,
                +0.5f, +0.5f, +0.5f,    +1.0f, +0.0f, +0.0f,
                +0.5f, -0.5f, +0.5f,    +1.0f, +0.0f, +0.0f,
                
                // 天面
                -0.5f, +0.5f, -0.5f,    +0.0f, +1.0f, +0.0f,
                -0.5f, +0.5f, +0.5f,    +0.0f, +1.0f, +0.0f,
                +0.5f, +0.5f, -0.5f,    +0.0f, +1.0f, +0.0f,
                +0.5f, +0.5f, -0.5f,    +0.0f, +1.0f, +0.0f,
                -0.5f, +0.5f, +0.5f,    +0.0f, +1.0f, +0.0f,
                +0.5f, +0.5f, +0.5f,    +0.0f, +1.0f, +0.0f,
                
                // 底面
                -0.5f, -0.5f, -0.5f,    +0.0f, -1.0f, +0.0f,
                +0.5f, -0.5f, -0.5f,    +0.0f, -1.0f, +0.0f,
                -0.5f, -0.5f, +0.5f,    +0.0f, -1.0f, +0.0f,
                -0.5f, -0.5f, +0.5f,    +0.0f, -1.0f, +0.0f,
                +0.5f, -0.5f, -0.5f,    +0.0f, -1.0f, +0.0f,
                +0.5f, -0.5f, +0.5f,    +0.0f, -1.0f, +0.0f,
            };
            
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vdata.size(), vdata.data(), GL_STATIC_DRAW);
            
            boxVertexCount = vdata.size() / 6;
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
        }
        
        GLsizei stride = sizeof(float) * 6;
        float *ptr = nullptr;
        glEnableVertexAttribArray(positionAttribId);
        glVertexAttribPointer(positionAttribId, 3, GL_FLOAT, GL_FALSE, stride, ptr);
        glEnableVertexAttribArray(normalAttribId);
        glVertexAttribPointer(normalAttribId, 3, GL_FLOAT, GL_FALSE, stride, ptr + 3);
        
        currentObjectVertexCount = boxVertexCount;
    }

    static void UseBevelBox(GLuint const positionAttribId, GLuint const normalAttribId)
    {
        static GLuint vbo = 0;
        
        if (!vbo) {
            std::vector<float> vdata = {
                +0.45f, +0.50f, +0.45f, +0.00f, +1.00f, +0.00f,
                +0.45f, +0.50f, -0.45f, +0.00f, +1.00f, +0.00f,
                -0.45f, +0.50f, +0.45f, +0.00f, +1.00f, +0.00f,
                -0.45f, +0.50f, +0.45f, +0.00f, +1.00f, +0.00f,
                +0.45f, +0.50f, -0.45f, +0.00f, +1.00f, +0.00f,
                -0.45f, +0.50f, -0.45f, +0.00f, +1.00f, +0.00f,
                
                +0.45f, -0.50f, -0.45f, -0.00f, -1.00f, -0.00f,
                +0.45f, -0.50f, +0.45f, -0.00f, -1.00f, -0.00f,
                -0.45f, -0.50f, -0.45f, -0.00f, -1.00f, -0.00f,
                -0.45f, -0.50f, -0.45f, -0.00f, -1.00f, -0.00f,
                +0.45f, -0.50f, +0.45f, -0.00f, -1.00f, -0.00f,
                -0.45f, -0.50f, +0.45f, -0.00f, -1.00f, -0.00f,
                
                -0.50f, -0.45f, -0.45f, -1.00f, +0.00f, -0.00f,
                -0.50f, -0.45f, +0.45f, -1.00f, +0.00f, -0.00f,
                -0.50f, +0.45f, -0.45f, -1.00f, +0.00f, -0.00f,
                -0.50f, +0.45f, -0.45f, -1.00f, +0.00f, -0.00f,
                -0.50f, -0.45f, +0.45f, -1.00f, +0.00f, -0.00f,
                -0.50f, +0.45f, +0.45f, -1.00f, +0.00f, -0.00f,
                
                +0.45f, +0.45f, +0.50f, +0.00f, -0.00f, +1.00f,
                -0.45f, +0.45f, +0.50f, +0.00f, -0.00f, +1.00f,
                +0.45f, -0.45f, +0.50f, +0.00f, -0.00f, +1.00f,
                +0.45f, -0.45f, +0.50f, +0.00f, -0.00f, +1.00f,
                -0.45f, +0.45f, +0.50f, +0.00f, -0.00f, +1.00f,
                -0.45f, -0.45f, +0.50f, +0.00f, -0.00f, +1.00f,
                
                +0.50f, +0.45f, -0.45f, +1.00f, -0.00f, +0.00f,
                +0.50f, +0.45f, +0.45f, +1.00f, -0.00f, +0.00f,
                +0.50f, -0.45f, -0.45f, +1.00f, -0.00f, +0.00f,
                +0.50f, -0.45f, -0.45f, +1.00f, -0.00f, +0.00f,
                +0.50f, +0.45f, +0.45f, +1.00f, -0.00f, +0.00f,
                +0.50f, -0.45f, +0.45f, +1.00f, -0.00f, +0.00f,
                
                +0.50f, +0.45f, -0.45f, +0.70f, +0.00f, -0.70f,
                +0.50f, -0.45f, -0.45f, +0.70f, +0.00f, -0.70f,
                +0.45f, +0.45f, -0.50f, +0.70f, +0.00f, -0.70f,
                +0.45f, +0.45f, -0.50f, +0.70f, +0.00f, -0.70f,
                +0.50f, -0.45f, -0.45f, +0.70f, +0.00f, -0.70f,
                +0.45f, -0.45f, -0.50f, +0.70f, +0.00f, -0.70f,
                
                -0.45f, +0.50f, -0.45f, +0.00f, +0.70f, -0.70f,
                +0.45f, +0.50f, -0.45f, +0.00f, +0.70f, -0.70f,
                -0.45f, +0.45f, -0.50f, +0.00f, +0.70f, -0.70f,
                -0.45f, +0.45f, -0.50f, +0.00f, +0.70f, -0.70f,
                +0.45f, +0.50f, -0.45f, +0.00f, +0.70f, -0.70f,
                +0.45f, +0.45f, -0.50f, +0.00f, +0.70f, -0.70f,
                
                +0.45f, +0.50f, -0.45f, +0.70f, +0.70f, +0.00f,
                +0.45f, +0.50f, +0.45f, +0.70f, +0.70f, +0.00f,
                +0.50f, +0.45f, -0.45f, +0.70f, +0.70f, +0.00f,
                +0.50f, +0.45f, -0.45f, +0.70f, +0.70f, +0.00f,
                +0.45f, +0.50f, +0.45f, +0.70f, +0.70f, +0.00f,
                +0.50f, +0.45f, +0.45f, +0.70f, +0.70f, +0.00f,
                
                +0.45f, -0.50f, -0.45f, -0.00f, -0.70f, -0.70f,
                -0.45f, -0.50f, -0.45f, -0.00f, -0.70f, -0.70f,
                +0.45f, -0.45f, -0.50f, -0.00f, -0.70f, -0.70f,
                +0.45f, -0.45f, -0.50f, -0.00f, -0.70f, -0.70f,
                -0.45f, -0.50f, -0.45f, -0.00f, -0.70f, -0.70f,
                -0.45f, -0.45f, -0.50f, -0.00f, -0.70f, -0.70f,
                
                +0.50f, -0.45f, -0.45f, +0.70f, -0.70f, +0.00f,
                +0.50f, -0.45f, +0.45f, +0.70f, -0.70f, +0.00f,
                +0.45f, -0.50f, -0.45f, +0.70f, -0.70f, +0.00f,
                +0.45f, -0.50f, -0.45f, +0.70f, -0.70f, +0.00f,
                +0.50f, -0.45f, +0.45f, +0.70f, -0.70f, +0.00f,
                +0.45f, -0.50f, +0.45f, +0.70f, -0.70f, +0.00f,
                
                -0.50f, -0.45f, -0.45f, -0.70f, +0.00f, -0.70f,
                -0.50f, +0.45f, -0.45f, -0.70f, +0.00f, -0.70f,
                -0.45f, -0.45f, -0.50f, -0.70f, +0.00f, -0.70f,
                -0.45f, -0.45f, -0.50f, -0.70f, +0.00f, -0.70f,
                -0.50f, +0.45f, -0.45f, -0.70f, +0.00f, -0.70f,
                -0.45f, +0.45f, -0.50f, -0.70f, +0.00f, -0.70f,
                
                -0.45f, -0.50f, -0.45f, -0.70f, -0.70f, -0.00f,
                -0.45f, -0.50f, +0.45f, -0.70f, -0.70f, -0.00f,
                -0.50f, -0.45f, -0.45f, -0.70f, -0.70f, -0.00f,
                -0.50f, -0.45f, -0.45f, -0.70f, -0.70f, -0.00f,
                -0.45f, -0.50f, +0.45f, -0.70f, -0.70f, -0.00f,
                -0.50f, -0.45f, +0.45f, -0.70f, -0.70f, -0.00f,
                
                -0.50f, +0.45f, -0.45f, -0.70f, +0.70f, +0.00f,
                -0.50f, +0.45f, +0.45f, -0.70f, +0.70f, +0.00f,
                -0.45f, +0.50f, -0.45f, -0.70f, +0.70f, +0.00f,
                -0.45f, +0.50f, -0.45f, -0.70f, +0.70f, +0.00f,
                -0.50f, +0.45f, +0.45f, -0.70f, +0.70f, +0.00f,
                -0.45f, +0.50f, +0.45f, -0.70f, +0.70f, +0.00f,
                
                +0.45f, +0.45f, +0.50f, +0.70f, -0.00f, +0.70f,
                +0.45f, -0.45f, +0.50f, +0.70f, -0.00f, +0.70f,
                +0.50f, +0.45f, +0.45f, +0.70f, -0.00f, +0.70f,
                +0.50f, +0.45f, +0.45f, +0.70f, -0.00f, +0.70f,
                +0.45f, -0.45f, +0.50f, +0.70f, -0.00f, +0.70f,
                +0.50f, -0.45f, +0.45f, +0.70f, -0.00f, +0.70f,
                
                +0.45f, +0.50f, +0.45f, +0.00f, +0.70f, +0.70f,
                -0.45f, +0.50f, +0.45f, +0.00f, +0.70f, +0.70f,
                +0.45f, +0.45f, +0.50f, +0.00f, +0.70f, +0.70f,
                +0.45f, +0.45f, +0.50f, +0.00f, +0.70f, +0.70f,
                -0.45f, +0.50f, +0.45f, +0.00f, +0.70f, +0.70f,
                -0.45f, +0.45f, +0.50f, +0.00f, +0.70f, +0.70f,
                
                +0.45f, -0.45f, +0.50f, -0.00f, -0.70f, +0.70f,
                -0.45f, -0.45f, +0.50f, -0.00f, -0.70f, +0.70f,
                +0.45f, -0.50f, +0.45f, -0.00f, -0.70f, +0.70f,
                +0.45f, -0.50f, +0.45f, -0.00f, -0.70f, +0.70f,
                -0.45f, -0.45f, +0.50f, -0.00f, -0.70f, +0.70f,
                -0.45f, -0.50f, +0.45f, -0.00f, -0.70f, +0.70f,
                
                -0.45f, -0.45f, +0.50f, -0.70f, +0.00f, +0.70f,
                -0.45f, +0.45f, +0.50f, -0.70f, +0.00f, +0.70f,
                -0.50f, -0.45f, +0.45f, -0.70f, +0.00f, +0.70f,
                -0.50f, -0.45f, +0.45f, -0.70f, +0.00f, +0.70f,
                -0.45f, +0.45f, +0.50f, -0.70f, +0.00f, +0.70f,
                -0.50f, +0.45f, +0.45f, -0.70f, +0.00f, +0.70f,
                
                +0.45f, +0.45f, -0.50f, +0.00f, +0.00f, -1.00f,
                +0.45f, -0.45f, -0.50f, +0.00f, +0.00f, -1.00f,
                -0.45f, +0.45f, -0.50f, +0.00f, +0.00f, -1.00f,
                -0.45f, +0.45f, -0.50f, +0.00f, +0.00f, -1.00f,
                +0.45f, -0.45f, -0.50f, +0.00f, +0.00f, -1.00f,
                -0.45f, -0.45f, -0.50f, +0.00f, +0.00f, -1.00f,
                
                +0.45f, +0.50f, -0.45f, +0.58f, +0.58f, -0.58f,
                +0.50f, +0.45f, -0.45f, +0.58f, +0.58f, -0.58f,
                +0.45f, +0.45f, -0.50f, +0.58f, +0.58f, -0.58f,
                
                +0.45f, -0.45f, -0.50f, +0.58f, -0.58f, -0.58f,
                +0.50f, -0.45f, -0.45f, +0.58f, -0.58f, -0.58f,
                +0.45f, -0.50f, -0.45f, +0.58f, -0.58f, -0.58f,
                
                -0.45f, -0.45f, -0.50f, -0.58f, -0.58f, -0.58f,
                -0.45f, -0.50f, -0.45f, -0.58f, -0.58f, -0.58f,
                -0.50f, -0.45f, -0.45f, -0.58f, -0.58f, -0.58f,
                
                -0.50f, +0.45f, -0.45f, -0.58f, +0.58f, -0.58f,
                -0.45f, +0.50f, -0.45f, -0.58f, +0.58f, -0.58f,
                -0.45f, +0.45f, -0.50f, -0.58f, +0.58f, -0.58f,
                
                +0.50f, +0.45f, +0.45f, +0.58f, +0.58f, +0.58f,
                +0.45f, +0.50f, +0.45f, +0.58f, +0.58f, +0.58f,
                +0.45f, +0.45f, +0.50f, +0.58f, +0.58f, +0.58f,
                
                +0.45f, -0.45f, +0.50f, +0.58f, -0.58f, +0.58f,
                +0.45f, -0.50f, +0.45f, +0.58f, -0.58f, +0.58f,
                +0.50f, -0.45f, +0.45f, +0.58f, -0.58f, +0.58f,
                
                -0.45f, -0.45f, +0.50f, -0.58f, -0.58f, +0.58f,
                -0.50f, -0.45f, +0.45f, -0.58f, -0.58f, +0.58f,
                -0.45f, -0.50f, +0.45f, -0.58f, -0.58f, +0.58f,
                
                -0.45f, +0.50f, +0.45f, -0.58f, +0.58f, +0.58f,
                -0.50f, +0.45f, +0.45f, -0.58f, +0.58f, +0.58f,
                -0.45f, +0.45f, +0.50f, -0.58f, +0.58f, +0.58f,
            };
            
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vdata.size(), vdata.data(), GL_STATIC_DRAW);
            
            bevelBoxVertexCount = vdata.size() / 6;
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
        }
        
        GLsizei stride = sizeof(float) * 6;
        float *ptr = nullptr;
        glEnableVertexAttribArray(positionAttribId);
        glVertexAttribPointer(positionAttribId, 3, GL_FLOAT, GL_FALSE, stride, ptr);
        glEnableVertexAttribArray(normalAttribId);
        glVertexAttribPointer(normalAttribId, 3, GL_FLOAT, GL_FALSE, stride, ptr + 3);
        
        currentObjectVertexCount = bevelBoxVertexCount;
    }
    
    static void UseBackground(GLuint const positionAttribId, GLuint const normalAttribId, int const innerWidth = 10, int const innerHeight = 20)
    {
        static GLuint vbo = 0;
        
        if (!vbo) {
            std::vector<float> const plane = {
                -0.5f, +0.5f, +0.0f,    +0.0f, +0.0f, +1.0f,
                -0.5f, -0.5f, +0.0f,    +0.0f, +0.0f, +1.0f,
                +0.5f, +0.5f, +0.0f,    +0.0f, +0.0f, +1.0f,
                +0.5f, +0.5f, +0.0f,    +0.0f, +0.0f, +1.0f,
                -0.5f, -0.5f, +0.0f,    +0.0f, +0.0f, +1.0f,
                +0.5f, -0.5f, +0.0f,    +0.0f, +0.0f, +1.0f,
            };
            
            glm::mat4 m;
            std::vector<glm::mat4> transforms;
            
            m = glm::translate(glm::mat4(), glm::vec3(0, 1.0f, -0.5f));
            m = glm::scale(m, glm::vec3(innerWidth, innerHeight + 2.0f, 1.0f));
            transforms.push_back(m);
            
            m = glm::translate(glm::mat4(), glm::vec3(-(innerWidth / 2.0f), 1.0f, 0.0f));
            m = glm::scale(m, glm::vec3(1.0f, innerHeight + 2.0f, 1.0f));
            m = glm::rotate(m, glm::pi<float>() / 2.0f, glm::vec3(0, 1, 0));
            transforms.push_back(m);
            
            m = glm::translate(glm::mat4(), glm::vec3(+(innerWidth / 2.0f), 1.0f, 0.0f));
            m = glm::scale(m, glm::vec3(1.0f, innerHeight + 2.0f, 1.0f));
            m = glm::rotate(m, glm::pi<float>() / 2.0f, glm::vec3(0, -1, 0));
            transforms.push_back(m);
            
            m = glm::translate(glm::mat4(), glm::vec3(0.0f, -(innerHeight / 2.0f), 0.0f));
            m = glm::scale(m, glm::vec3(innerWidth, 1.0f, 1.0f));
            m = glm::rotate(m, glm::pi<float>() / 2.0f, glm::vec3(-1, 0, 0));
            transforms.push_back(m);
            
            m = glm::translate(glm::mat4(), glm::vec3(-(innerWidth / 2.0f + 5.0f), 1.0f, 0.5f));
            m = glm::scale(m, glm::vec3(10.0f, innerHeight + 2.0f, 1.0f));
            transforms.push_back(m);
            
            m = glm::translate(glm::mat4(), glm::vec3(+(innerWidth / 2.0f + 5.0f), 1.0f, 0.5f));
            m = glm::scale(m, glm::vec3(10.0f, innerHeight + 2.0f, 1.0f));
            transforms.push_back(m);
            
            m = glm::translate(glm::mat4(), glm::vec3(0.0f, -(innerHeight / 2.0f + 2.0f), 0.5f));
            m = glm::scale(m, glm::vec3(innerWidth + 20.0f, 4.0f, 1.0f));
            transforms.push_back(m);
            
            std::vector<float> vdata;
            for (auto m: transforms) {
                for (int i = 0; i < plane.size() / 6; ++i) {
                    glm::vec3 p = glm::make_vec3(&(plane.data()[i * 6 + 0]));
                    glm::vec3 n = glm::make_vec3(&(plane.data()[i * 6 + 3]));
                    
                    p = glm::vec3(m * glm::vec4(p, 1.0f));
                    n = glm::mat3(m) * n;
                    
                    vdata.push_back(p.x); vdata.push_back(p.y); vdata.push_back(p.z);
                    vdata.push_back(n.x); vdata.push_back(n.y); vdata.push_back(n.z);
                }
            }
            
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vdata.size(), vdata.data(), GL_STATIC_DRAW);
            
            backgroundVertexCount = vdata.size() / 6;
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
        }
        
        GLsizei stride = sizeof(float) * 6;
        float *ptr = nullptr;
        glEnableVertexAttribArray(positionAttribId);
        glVertexAttribPointer(positionAttribId, 3, GL_FLOAT, GL_FALSE, stride, ptr);
        glEnableVertexAttribArray(normalAttribId);
        glVertexAttribPointer(normalAttribId, 3, GL_FLOAT, GL_FALSE, stride, ptr + 3);
        
        currentObjectVertexCount = backgroundVertexCount;
    }
    
    static void Draw()
    {
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)currentObjectVertexCount);
    }
    
    static std::vector<BoxChar> GetBoxChars(std::string const str)
    {
        if (charMap.size() == 0) {
            int const height = 5;
            std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!? @_0123456789^v<>:.";
            std::vector<std::string> cd = {
                " # !## !###!## !###!###!###!# #!#!  #!# #!#  !#   #!#  #!",
                "# # # # # # # # #   #   #   # # #   # # # #   ## ## ## # ",
                "### ##  #   # # ### ### # # ### #   # ##  #   # # # # ## ",
                "# # # # # # # # #   #   # # # # # # # # # #   #   # #  # ",
                "# # ##  ### ##  ### #   ### # # # ### # # ### #   # #  # ",
                
                "###!###!###!###!###!###!# #!# #!# # #!# #!# #!###!#!###! ! ###!   !",
                "# # # # # # # # #    #  # # # # # # # # # # #   # #   #   # ##     ",
                "# # ### # # ##  ###  #  # # # # # # #  #   #   #  #  #    # ##     ",
                "# # #   ##  # #   #  #  # # # # # # # # #  #  #           #        ",
                "### #    ## # # ###  #  ###  #   # #  # #  #  ### #  #    #### ### ",
                
                "###!## !###!###!# #!###!###!###!###!###!  #  !  #  !  #  !  #  ! ! !",
                "# #  #    #   # # # #   #     # # # # #  ###    #    #       #  #   ",
                "# #  #  ### ### ### ### ###   # ### ### # # # # # # ##### #####     ",
                "# #  #  #     #   #   # # #   # # #   #   #    ###   #       #  #   ",
                "### ### ### ###   # ### ###   # ### ###   #     #     #     #     # ",
            };
            
            auto curId = 0;
            auto curLine = 0;
            auto curCol = 0;
            while (curId < chars.length() && curLine * height < cd.size()) {
                auto curRow = curLine * height;
                auto charEnd = curCol + 1;
                while (cd[curRow][charEnd] != '!') ++charEnd;
                
                BoxChar bc;
                bc.character = chars[curId];
                bc.width = charEnd - curCol;
                
                glm::vec2 center(float(bc.width) / 2.0f, float(height) / 2.0f);
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < bc.width; ++x) {
                        if (cd[curRow + y][curCol + x] == '#') {
                            auto p = glm::vec2(x + 0.5f, y + 0.5f) - center;
                            bc.dotData.push_back(glm::vec2(p.x, -p.y));
                        }
                    }
                }
                
                charMap[bc.character] = bc;
                ++curId;
                curCol = charEnd + 1;
                if (curCol >= cd[curRow].length()) {
                    curCol = 0;
                    ++curLine;
                }
            }
        }
        
        std::vector<BoxChar> boxChars;
        for (auto c: str) {
            boxChars.push_back(charMap[c]);
        }
        return std::move(boxChars);
    }
    
    enum CenterType {
        LEFT_CENTER, RIGHT_CENTER, CENTER
    };
    
    static std::vector<glm::mat4> GetStringMatrices(std::string const str, CenterType const centerType = CenterType::CENTER)
    {
        if (str.length() == 0) return std::move(std::vector<glm::mat4>());
        
        auto bcs = GetBoxChars(str);
        
        int totalWidth = 0;
        for (auto &bc: bcs) totalWidth += bc.width + 1;
        totalWidth -= 1;
        
        float offsetX = 0.0f;
        switch (centerType) {
            case CenterType::LEFT_CENTER:
                break;
            case CenterType::RIGHT_CENTER:
                offsetX = -float(totalWidth);
                break;
            case CenterType::CENTER:
                offsetX = -float(totalWidth) * 0.5f;
                break;
        }
        
        std::vector<glm::mat4> matrices;
        int charX = 0;
        for (auto &bc: bcs) {
            auto width = bc.width;
            glm::vec2 offset(float(charX) + (float(width) / 2.0f) + offsetX, 0.0f);
            for (auto &p: bc.dotData) {
                auto strLocalPos = p + offset;
                glm::mat4 m = glm::translate(glm::mat4(), glm::vec3(strLocalPos, 0.0f));
                matrices.push_back(m);
            }
            
            charX += width + 1;
        }
        
        return std::move(matrices);
    }
};

size_t ObjectData::boxVertexCount = 0;
size_t ObjectData::bevelBoxVertexCount = 0;
size_t ObjectData::backgroundVertexCount = 0;
size_t ObjectData::currentObjectVertexCount = 0;
ObjectData::CharMap ObjectData::charMap;

#endif
