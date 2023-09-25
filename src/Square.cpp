//
// Created by Bojana on 01-Sep-23.
//

#include <Square.h>

Square::Square() {}
Square::Square(int32_t r, int32_t c, SQUARE_TYPE t){
    this->x = r;
    this->y = c;
    this->sType = t;
    this->fScore = INT_MAX;
    this->gScore = INT_MAX;
    this->parentY = -1;
    this->parentX = -1;
    this->insertionOrder = -1;
}
Square::~Square() {}

glm::vec4 Square::color() {
    if(sType == NORMAL)
        return glm::vec4 (1.0f,1.0f,1.0f,1.0f);
    else if(sType == START)
        return glm::vec4 (1.0f,1.0f,0.0f,1.0f); // start point
    else if(sType == END)
        return glm::vec4 (0.2f,0.2f,1.0f,1.0f); //end point
    else if(sType == BARRIER)
        return glm::vec4 (0.0f,0.0f,0.0f,1.0f); // barrier
    else if (sType == PATH)
        return glm::vec4 (1.0f,0.0f,1.0f,1.0f); // path
}

bool Square::operator < (const Square &other) const {
    if (this->fScore == other.fScore)
        return insertionOrder > other.insertionOrder;

    return this->fScore > other.fScore;
}



