//
// Created by Bojana on 01-Sep-23.
//

#include <Square.h>

Square::Square() {}
Square::Square(uint32_t r, uint32_t c, SQUARE_TYPE t){
    this->x = r;
    this->y = c;
    this->sType = t;
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
    else if(sType == CLOSED)
        return glm::vec4 (1.0f,0.0f,0.0f,1.0f); // closed set
    else if (sType == OPEN)
        return glm::vec4 (0.0f,1.0f,1.0f,1.0f); //open set
    else
        return glm::vec4 (1.0f,0.0f,1.0f,1.0f); // path


}




