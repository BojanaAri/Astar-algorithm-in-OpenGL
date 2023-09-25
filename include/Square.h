//
// Created by Bojana on 01-Sep-23.
//

#ifndef OPENGLPRJ_SQUARE_H
#define OPENGLPRJ_SQUARE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>

enum SQUARE_TYPE { NORMAL, BARRIER, START, END, PATH };

class Square{
public:
    SQUARE_TYPE sType;
    int32_t x, y;
    int32_t parentX, parentY;
    int fScore, gScore, insertionOrder;

    Square();
    Square(int32_t , int32_t , SQUARE_TYPE);
    ~Square();

    glm::vec4 color();
    bool operator < (const Square &other) const;
};


#endif //OPENGLPRJ_SQUARE_H
