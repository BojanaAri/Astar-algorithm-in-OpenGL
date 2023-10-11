//
// Created by Bojana on 11-Oct-23.
//

#ifndef OPENGLPRJ_RAYOBJECT_H
#define OPENGLPRJ_RAYOBJECT_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class RayObject {
public:
    RayObject();
    ~RayObject();
    void getRayFromMouse(double xPos, double yPos, unsigned int SCR_WIDTH,unsigned int SCR_HEIGHT, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::vec3 &rayDirection);
    bool testRayOBBIntersection(
            glm::vec3 rayOrigin,        // Ray origin, in world space
            glm::vec3 rayDirection,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
            glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
            glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
            glm::mat4 modelMatrix      // Transformation applied to the mesh (which will thus be also applied to its bounding box)
    );
};


#endif //OPENGLPRJ_RAYOBJECT_H
