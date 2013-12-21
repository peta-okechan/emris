PROJECT(emris)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

SET(GLUT_INCLUDE_DIR "/usr/local/Cellar/freeglut/2.8.1/include")
SET(GLM_INCLUDE_DIR "/usr/local/Cellar/glm/0.9.4.6/include")

FIND_PACKAGE(GLUT)
FIND_PACKAGE(OpenGL)

INCLUDE_DIRECTORIES(
    ${GLUT_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)

ADD_EXECUTABLE(emris src/main.cpp)

TARGET_LINK_LIBRARIES(
    emris
    ${OPENGL_LIBRARY}
    ${GLUT_LIBRARY}
)

ADD_CUSTOM_TARGET(
    emscripten
    em++
    -std=c++11
    -s FULL_ES2=1
    -s GL_TESTING=1
    -O2
    -I ${GLM_INCLUDE_DIR}
    -o emris.html
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)