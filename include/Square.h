//
// Created by Bojana on 01-Sep-23.
//

#ifndef OPENGLPRJ_SQUARE_H
#define OPENGLPRJ_SQUARE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <queue>
#include <vector>

enum SQUARE_TYPE { NORMAL, OPEN, CLOSED, BARRIER, START, END };

class Square{
public:
    SQUARE_TYPE sType;
    uint32_t x, y;

    Square();
    Square(uint32_t , uint32_t , SQUARE_TYPE);
    ~Square();

    glm::vec4 color();



};


#endif //OPENGLPRJ_SQUARE_H
