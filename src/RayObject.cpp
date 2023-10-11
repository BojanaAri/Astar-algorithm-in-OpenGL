//
// Created by Bojana on 11-Oct-23.
//

#include "RayObject.h"

RayObject::RayObject() = default;
RayObject::~RayObject() = default;

void RayObject::getRayFromMouse(double xPos, double yPos, unsigned int SCR_WIDTH,unsigned int SCR_HEIGHT, glm::mat4 projectionMatrix, glm::mat4 viewMatrix,
                                glm::vec3 &rayDirection) {
    // We define the start & end point of the ray that will be casted through the screen
    glm::vec4 RayPointStart_NDC(
            ((float)xPos/(float)SCR_WIDTH  - 0.5f) * 2.0f, // [0, 800] -> [-1,1]
            ((float)yPos/(float)SCR_HEIGHT - 0.5f) * 2.0f, // [0, 800] -> [-1,1]
            -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
            1.0f);

    glm::vec4 RayPointEnd_NDC(
            ((float)xPos/(float)SCR_WIDTH  - 0.5f) * 2.0f,
            ((float)yPos/(float)SCR_HEIGHT - 0.5f) * 2.0f,
            0.0,
            1.0f);

    // Inverse (ProjectionMatrix) goes from Clip Space to Camera Space,
    // While inverse(ViewMatrix) goes from Camera Space to World Space.
    glm::mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);
    glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);


    glm::vec4 RayStartCamera = inverseProjectionMatrix * RayPointStart_NDC;
    RayStartCamera /= RayStartCamera.w;
    glm::vec4 RayStartWorld  = inverseViewMatrix * RayStartCamera;
    RayStartWorld /= RayStartWorld .w;


    glm::vec4 RayEndCamera  = inverseProjectionMatrix * RayPointEnd_NDC;
    RayEndCamera  /= RayEndCamera  .w;
    glm::vec4 RayEndWorld    = inverseViewMatrix * RayEndCamera;
    RayEndWorld   /= RayEndWorld   .w;



    glm::vec3 RayDirectionWorld(RayEndWorld - RayStartWorld);
    RayDirectionWorld = glm::normalize(RayDirectionWorld);

    rayDirection = RayDirectionWorld;
}

bool RayObject::testRayOBBIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 aabb_min,
                                       glm::vec3 aabb_max, glm::mat4 modelMatrix) {

    float tMin = 0.0f;
    float tMax = 100000.0f;

    glm::vec3 OBBposition_worldspace(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);
    glm::vec3 delta = OBBposition_worldspace - rayOrigin;


    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(rayDirection, xaxis);

        if ( fabs(f) > 0.001f ){ // Standard case

            float t1 = (e+aabb_min.x)/f; // Intersection with the "left" plane
            float t2 = (e+aabb_max.x)/f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection,
            // so if it's not the case, invert t1 and t2
            if (t1>t2){
                float w=t1;t1=t2;t2=w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if ( t2 < tMax )
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if ( t1 > tMin )
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin )
                return false;

        }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if(-e+aabb_min.x > 0.0f || -e+aabb_max.x < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(modelMatrix[1].x, modelMatrix[1].y, modelMatrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(rayDirection, yaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.y)/f;
            float t2 = (e+aabb_max.y)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.y > 0.0f || -e+aabb_max.y < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(modelMatrix[2].x, modelMatrix[2].y, modelMatrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(rayDirection, zaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.z)/f;
            float t2 = (e+aabb_max.z)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.z > 0.0f || -e+aabb_max.z < 0.0f)
                return false;
        }
    }
    return true;
}