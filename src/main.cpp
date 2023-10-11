#include <OpenGLPrj.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Shader.hpp>
#include <Square.h>
#include <Camera.hpp>
#include <RayObject.h>

#include <iostream>
#include <string>
#include <queue>

using namespace std;

const std::string program_name = ("A* algorithm");

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void makeGrid(::int32_t, ::int32_t);
int heuristicFunction(const Square& m1, const Square& m2);
void makePath(::int32_t x, ::int32_t y);
bool algorithm();

// variables
int32_t nr, nc, rows = 20, cols = 20;
Square m[20][20];
Square startSquare, endSquare;

bool start = true;
bool End = true;

// -----------------------------------------------------------------------------
//                                          Camera
static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f; // time between current frame and last frame
static float lastFrame = 0.0f;

// lighting
static glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                          program_name.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    //--------------------------------------------------------------------------------------------------
    //
    std::string shader_location("../res/shaders/");

    std::string lamp_shader("lamp");
    std::string used_shaders("shader");
    std::string path("path");

    Shader ourShader(shader_location + used_shaders + std::string(".vert"),
                     shader_location + used_shaders + std::string(".frag"));

    Shader lampShader(shader_location + lamp_shader + std::string(".vert"),
                      shader_location + lamp_shader + std::string(".frag"));

    Shader pathShader(shader_location + path + std::string(".vert"),
                shader_location + path + std::string(".frag"));

    ////////////////////////////////////////////////////////////////////////////////////

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    ///////////////////
    float vertices[] = {
            -0.05f, -0.05f, -0.05f, 0.0f,  0.0f,  -1.0f, 0.05f,  -0.05f, -0.05f,
            0.0f,  0.0f,-1.0f, 0.05f,  0.05f,  -0.05f, 0.0f,  0.0f,  -1.0f,
            0.05f,0.05f,  -0.05f, 0.0f,  0.0f,-1.0f, -0.05f, 0.05f,  -0.05f,
            0.0f,  0.0f,  -1.0f,-0.05f, -0.05f, -0.05f, 0.0f,  0.0f,  -1.0f,

            -0.05f, -0.05f, 0.05f,  0.0f,  0.0f,  1.0f,  0.05f,  -0.05f, 0.05f,
            0.0f,  0.0f,  1.0f,  0.05f,   0.05f,  0.05f,  0.0f,  0.0f,  1.0f,
            0.05f,  0.05f,  0.05f,  0.0f,  0.0f,  1.0f,  -0.05f, 0.05f,  0.05f,
            0.0f,  0.0f,  1.0f,  -0.05f, -0.05f, 0.05f,  0.0f,  0.0f,  1.0f,

            -0.05f, 0.05f,  0.05f,  -1.0f, 0.0f,  0.0f,  -0.05f, 0.05f,  -0.05f,
            -1.0f, 0.0f,  0.0f,  -0.05f, -0.05f, -0.05f, -1.0f, 0.0f,  0.0f,
            -0.05f, -0.05f, -0.05f, -1.0f, 0.0f,  0.0f,  -0.05f, -0.05f, 0.05f,
            -1.0f, 0.0f,  0.0f,  -0.05f, 0.05f,  0.05f,  -1.0f, 0.0f,  0.0f,

            0.05f,  0.05f,  0.05f,  1.0f,0.0f,  0.0f,  0.05f,  0.05f,  -0.05f,
            1.0f,  0.0f,  0.0f,  0.05f,  -0.05f, -0.05f, 1.0f,  0.0f,  0.0f,
            0.05f,  -0.05f, -0.05f, 1.0f,  0.0f,  0.0f,  0.05f,  -0.05f, 0.05f,
            1.0f,  0.0f,  0.0f,  0.05f,  0.05f,  0.05f,  1.0f,  0.0f,  0.0f,

            -0.05f, -0.05f, -0.05f, 0.0f,  -1.0f, 0.0f,  0.05f,  -0.05f, -0.05f,
            0.0f,  -1.0f, 0.0f,  0.05f,  -0.05f, 0.05f,  0.0f,  -1.0f, 0.0f,
            0.05f,  -0.05f, 0.05f,  0.0f,  -1.0f, 0.0f,  -0.05f, -0.05f, 0.05f,
            0.0f,  -1.0f, 0.0f,  -0.05f, -0.05f, -0.05f, 0.0f,  -1.0f, 0.0f,

            -0.05f, 0.05f,  -0.05f, 0.0f,  1.0f,  0.0f,  0.05f,  0.05f,  -0.05f,
            0.0f,  1.0f,  0.0f,  0.05f,  0.05f,  0.05f,  0.0f,  1.0f,  0.0f,
            0.05f,  0.05f,  0.05f,  0.0f,  1.0f,  0.0f,  -0.05f, 0.05f,  0.05f,
            0.0f,  1.0f,  0.0f,  -0.05f, 0.05f,  -0.05f, 0.0f,  1.0f,  0.0f};

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glBindVertexArray(VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    // second, configure the light's VAO (VBO stays the same; the vertices are the
    // same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the
    // updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);


    nr = nc = 8;
    makeGrid(rows,cols);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // change the light's position values over time (can be done anywhere in the
        // render loop actually, but try to do it at least before using the light
        // source positions)
        lightPos.x = 1.0f;
        lightPos.y = 2.0f;

        ourShader.use();
        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("viewPos", camera.Position);

        // create transformations
        glm::mat4 projection = camera.GetProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);
        glm::mat4 view = camera.GetViewMatrix();

        glBindVertexArray(VAO);

        ourShader.setMat4("projection",projection);
        ourShader.setMat4("view", view);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                glm::mat4 model = glm::mat4(1.0f);

                float xcoord = ((float) m[i][j].x / nr - 1.18f);
                float ycoord = 2.18-( (float) m[i][j].y / nc + 1.0f);

                model = glm::translate(model, glm::vec3(xcoord, ycoord, 0.0f));

                glm::vec4 uni;
                if(m[i][j].sType == SQUARE_TYPE::START) {
                    int st = glGetUniformLocation(ourShader.ID, "colorS");
                    uni = m[i][j].color();
                    glUniform4f(st, uni.x, uni.y, uni.z, uni.w);
                    start = false;
                    startSquare = m[i][j];
                }

                if(m[i][j].sType == SQUARE_TYPE::END) {
                    int e = glGetUniformLocation(ourShader.ID, "colorE");
                    uni = m[i][j].color();
                    glUniform4f(e, uni.x, uni.y, uni.z, uni.w);
                    End = false;
                    endSquare = m[i][j];
                }

                if(m[i][j].sType == SQUARE_TYPE::PATH) {
                    pathShader.use();

                    pathShader.setMat4("projection", projection);
                    pathShader.setMat4("view",view);

                    pathShader.setVec3("light.position", lightPos);
                    pathShader.setVec3("viewPos", camera.Position);

                    glm::vec3 lightColor;
                    lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
                    lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
                    lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));
                    glm::vec3 diffuseColor =
                            lightColor * glm::vec3(0.5f); // decrease the influence
                    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence

                    pathShader.setVec3("light.ambient", ambientColor);
                    pathShader.setVec3("light.diffuse", diffuseColor);
                    pathShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

                    pathShader.setVec3("material.ambient", 1.0f, 0.0f, 1.0f);
                    pathShader.setVec3("material.diffuse", 1.2f, 1.5f, 1.31f);
                    pathShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
                    pathShader.setFloat("material.shininess", 64.0f);

                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.15f));

                    pathShader.setMat4("model",model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    continue;
                }

                ourShader.use();
                if(m[i][j].sType == SQUARE_TYPE::BARRIER)
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.15f));


                int colorLocation = glGetUniformLocation(ourShader.ID, "color");
                uni = m[i][j].color();
                glUniform3f(colorLocation, uni.x , uni.y, uni.z);

                ourShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // lamp object
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        lampShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


         // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
        // etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    double xpos, ypos;
    if( glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){

        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec3 rayDirection,rayOrigin;

        RayObject::getRayFromMouse(xpos,SCR_HEIGHT-ypos,
                                    SCR_WIDTH, SCR_HEIGHT,
                                    camera.GetProjectionMatrix(SCR_WIDTH, SCR_HEIGHT),
                                    camera.GetViewMatrix(),
                                    rayDirection);
        rayOrigin = camera.Position;

        glm::vec3 aabb_min(-0.05f, -0.05f, -0.05f);
        glm::vec3 aabb_max( 0.05f,  0.05f,  0.05f);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                float xcoord = ((float) m[i][j].x / nr - 1.18f);
                float ycoord = 2.18-( (float) m[i][j].y / nc + 1.0f);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xcoord, ycoord, 0.0f));

                if(RayObject::testRayOBBIntersection(rayOrigin,
                                                    rayDirection,
                                                    aabb_min,
                                                    aabb_max,
                                                    model)){
                    if (start && m[i][j].sType != SQUARE_TYPE::END) {
                        m[i][j].sType = SQUARE_TYPE::START;
                        start = false;
                        return;
                    }
                    if (End && m[i][j].sType != SQUARE_TYPE::START) {
                        m[i][j].sType = SQUARE_TYPE::END;
                        End = false;
                        return;
                    }
                    if (m[i][j].sType != SQUARE_TYPE::START && m[i][j].sType != SQUARE_TYPE::END
                        &&  (i >= 0 && i < 20) && (j >= 0 && j < 20) )
                        m[i][j].sType = SQUARE_TYPE::BARRIER;

                    if(glfwGetKey(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && m[i][j].sType == SQUARE_TYPE::BARRIER)
                        m[i][j].sType = SQUARE_TYPE::NORMAL;

                    break;
                }
            }
        }
    }

    int startAlgorithm = glfwGetKey(window, GLFW_KEY_SPACE);
    if (startAlgorithm == GLFW_PRESS && !start && !End)
    {
         algorithm();
    }
    else if (startAlgorithm == GLFW_PRESS)
    {
        cout<<"Add start and end position"<<endl;
    }

    // restart the algorithm
    if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        makeGrid(rows,cols);
        start = true;
        End = true;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow *window, double xposd, double yposd) {
    float xpos = static_cast<float>(xposd);
    float ypos = static_cast<float>(yposd);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
            lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// ---------------------------------------------------------------------------------------------
//                                      MAKE GRID

void makeGrid(::int32_t r, ::int32_t c) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            Square x(i, j, SQUARE_TYPE::NORMAL);
            m[i][j] = x;
        }
    }
}
// --------------------------------------------------------------------------------------------
//                             MANHATTAN HEURISTIC FUNCTION

int heuristicFunction(const Square& m1, const Square& m2)
{
    int x1 = m1.x, y1 = m1.y;
    int x2 = m2.x, y2 = m2.y;
    return abs(x1-x2) + abs(y1-y2);
}

// ------------------------------------------------------------------------------------------
//                                       ALGORITHM

void makePath(::int32_t x, ::int32_t y)
{
    while( x != startSquare.x || y != startSquare.y)
    {
        m[x][y].sType = SQUARE_TYPE::PATH;
        int tempX = m[x][y].parentX;
        int tempY = m[x][y].parentY;
        x = tempX;
        y = tempY;
    }
}

int cnt = 0;
bool algorithm()
{
    bool closedList[20][20] = {{false}};

    m[startSquare.x][startSquare.y].fScore = heuristicFunction(startSquare, endSquare);
    m[startSquare.x][startSquare.y].gScore = 0;

    std::priority_queue<Square> openList;
    m[startSquare.x][startSquare.y].insertionOrder = cnt;

    openList.push(m[startSquare.x][startSquare.y]);

    while(!openList.empty()){
        Square current = openList.top();
        openList.pop();

        ::int32_t i = current.x, j = current.y;

        if(i == endSquare.x && j == endSquare.y) {
            makePath(current.x,current.y);
            m[endSquare.x][endSquare.y].sType = SQUARE_TYPE::END;
            return true;
        }

        closedList[i][j] = true;
        int tmpGscore = current.gScore + 1;

        if (i < 19 && m[i + 1][j].sType != SQUARE_TYPE::BARRIER && !closedList[i + 1][j])      // RIGHT
        {
            int fNew =  tmpGscore + heuristicFunction(m[i+1][j],endSquare);

            if(m[i+1][j].fScore == INT_MAX || m[i+1][j].fScore > fNew){
                m[i+1][j].parentX = i;
                m[i+1][j].parentY = j;

                m[i+1][j].gScore = tmpGscore;
                m[i+1][j].fScore = fNew;

                cnt++;
                m[i+1][j].insertionOrder = cnt;
                openList.push(m[i+1][j]);
            }
        }

        if(i > 0 && m[i-1][j].sType != SQUARE_TYPE::BARRIER && !closedList[i - 1][j]){      // LEFT
            int fNew =  tmpGscore + heuristicFunction(m[i-1][j],endSquare);

            if(m[i-1][j].fScore == INT_MAX || m[i-1][j].fScore > fNew)
            {
                m[i-1][j].parentX = i;
                m[i-1][j].parentY = j;

                m[i-1][j].gScore = tmpGscore;
                m[i-1][j].fScore = fNew;

                cnt++;
                m[i-1][j].insertionOrder = cnt;
                openList.push(m[i-1][j]);
            }
        }

        if (j < 19 && m[i][j+1].sType != SQUARE_TYPE::BARRIER && !closedList[i][j+1]){      // DOWN
            int fNew =  tmpGscore + heuristicFunction(m[i][j+1],endSquare);

            if(m[i][j+1].fScore == INT_MAX || m[i][j+1].fScore > fNew) {
                m[i][j + 1].parentX = i;
                m[i][j + 1].parentY = j;

                m[i][j + 1].gScore = tmpGscore;
                m[i][j + 1].fScore = fNew;

                cnt++;
                m[i][j+1].insertionOrder = cnt;
                openList.push(m[i][j+1]);
            }
        }

        if(j > 0 && m[i][j-1].sType != SQUARE_TYPE::BARRIER && !closedList[i][j-1]){     // UP
            int fNew =  tmpGscore + heuristicFunction(m[i][j-1],endSquare);

            if(m[i][j-1].fScore == INT_MAX || m[i][j-1].fScore > fNew){
                m[i][j-1].parentX = i;
                m[i][j-1].parentY = j;

                m[i][j-1].gScore = tmpGscore;
                m[i][j-1].fScore = fNew;

                cnt++;
                m[i][j-1].insertionOrder = cnt;
                openList.push(m[i][j-1]);
            }
        }
    }

    return false;
}
