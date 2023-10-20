#include <OpenGLPrj.hpp>
#include <GLFW/glfw3.h>
#include <Shader.hpp>
#include <Square.h>

#include <iostream>
#include <string>
#include <queue>

using namespace std;

const std::string program_name = ("A* algorithm");

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


//----------------------------------------------------------------------------------------------------------------------
//                                                 FUNCTIONS

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Shader shader, unsigned int VAO, unsigned int VBO, unsigned int EBO);
void makeGrid(::int32_t, ::int32_t);
int heuristicFunction(const Square& m1, const Square& m2);
void makePath(::int32_t x, ::int32_t y);
bool algorithm(GLFWwindow *window, Shader ourShader, unsigned int VAO, unsigned int VBO, unsigned int EBO);
void draw(GLFWwindow *window, Shader ourShader, unsigned int VAO, unsigned int VBO, unsigned int EBO);

// variables
int32_t nr, nc, rows = 20, cols = 20;
Square m[20][20];
Square startSquare, endSquare;

bool start = true;
bool End = true;


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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //--------------------------------------------------------------------------------------------------
     std::string shader_location("../res/shaders/");

        std::string used_shaders("shader");
        Shader ourShader(shader_location + used_shaders + std::string(".vert"),
                         shader_location + used_shaders + std::string(".frag"));


    float vertices[] = {
            // positions
            0.06f, 0.06f, 0.0f,        // top right
            0.06f, -0.06f, 0.0f,       // bottom right
            -0.06f, -0.06f, 0.0f,     // bottom left
            -0.06f, 0.06f, 0.0f,   // top left
    };

    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);


    nr = nc = 8;
    makeGrid(rows,cols);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
   processInput(window, ourShader, VAO, VBO, EBO);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // also clear the depth buffer now!

        draw(window, ourShader, VAO, VBO, EBO);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
        // etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void draw(GLFWwindow *window, Shader ourShader, unsigned int VAO, unsigned int VBO, unsigned int EBO){


    //processInput(window, ourShader, VAO, VBO, EBO);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); // also clear the depth buffer now!


    ourShader.use();

    // create transformations
    glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));


    // pass transformation matrices to the shader
    ourShader.setMat4("projection",projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    ourShader.setMat4("view", view);

    // render boxes
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            glm::mat4 model = glm::mat4(1.0f);

            float xcoord = ((float) m[i][j].x / nr - 1.18f);
            float ycoord = 2.18-( (float) m[i][j].y / nc + 1.0f);


            model = glm::translate(model, glm::vec3(xcoord, ycoord, 0.0f));
            ourShader.setMat4("model", model);

            glm::vec4 uni;
            if(m[i][j].sType == SQUARE_TYPE::START) {
                start = false;
                startSquare = m[i][j];
            }

            if(m[i][j].sType == SQUARE_TYPE::END) {
                End = false;
                endSquare = m[i][j];
            }

            if(m[i][j].sType == SQUARE_TYPE::PATH) {
                int e = glGetUniformLocation(ourShader.ID, "open");
                uni = m[i][j].color();
                glUniform4f(e, uni.x, uni.y, uni.z, uni.w);
            }


            int colorLocation = glGetUniformLocation(ourShader.ID, "color");
            uni = m[i][j].color();
            glUniform4f(colorLocation, uni.x , uni.y, uni.z, uni.w);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }
    }
}


// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow *window, Shader ourShader, unsigned int VAO, unsigned int VBO, unsigned int EBO) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    double xpos, ypos;

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
        glfwGetCursorPos(window, &xpos, &ypos);
        int x = (int) (xpos / 40.0f);
        int y = (int) (ypos / 40.0f);


        if (start && m[x][y].sType != SQUARE_TYPE::END) {
            m[x][y].sType = SQUARE_TYPE::START;
            start = false;
            return;
        }
        if (End && m[x][y].sType != SQUARE_TYPE::START) {
            m[x][y].sType = SQUARE_TYPE::END;
            End = false;
            return;
        }

        if (m[x][y].sType != SQUARE_TYPE::START && m[x][y].sType != SQUARE_TYPE::END
            &&  (x >= 0 && x < 20) && (y >= 0 && y < 20) )
            m[x][y].sType = SQUARE_TYPE::BARRIER;

    }

    int white = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (white == GLFW_PRESS) {
        glfwGetCursorPos(window, &xpos, &ypos);
        int x = (int) (xpos / 40.0f);
        int y = (int) (ypos / 40.0f);

        if (m[x][y].sType == SQUARE_TYPE::BARRIER)
            m[x][y].sType = SQUARE_TYPE::NORMAL;
    }

    int startAlgorithm = glfwGetKey(window, GLFW_KEY_SPACE);
    if (startAlgorithm == GLFW_PRESS && !start && !End)
    {
        algorithm(window,ourShader,VAO,VBO,EBO);
    }
    else if (startAlgorithm == GLFW_PRESS){
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
bool algorithm(GLFWwindow *window, Shader ourShader, unsigned int VAO, unsigned int VBO, unsigned int EBO)
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

        if (i < 19 && m[i + 1][j].sType != SQUARE_TYPE::BARRIER && !closedList[i + 1][j])      // DOWN
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

        if(i > 0 && m[i-1][j].sType != SQUARE_TYPE::BARRIER && !closedList[i - 1][j]){    // UP
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

        if (j < 19 && m[i][j+1].sType != SQUARE_TYPE::BARRIER && !closedList[i][j+1]){
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

        if(j > 0 && m[i][j-1].sType != SQUARE_TYPE::BARRIER && !closedList[i][j-1]){
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
        draw(window, ourShader, VAO,VBO,EBO);
    }

    return false;
}