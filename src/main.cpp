#include <OpenGLPrj.hpp>
#include <GLFW/glfw3.h>
#include <Shader.hpp>
#include <Square.h>

#include <iostream>
#include <string>
#include <list>
#include <queue>

using namespace std;

const std::string program_name = ("A* algorithm");

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


//----------------------------------------------------------------------------------------------------------------------
//                                                 FUNCTIONS

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void makeGrid(::int32_t, ::int32_t);
float heuristicFunction(const Square& m1, const Square& m2);
bool algorithm();
void makePath(::int32_t x, ::int32_t y);
bool aStar();

// settings


// variables
uint32_t nr, nc, rows = 20, cols = 20;
Square m[20][20];
Square startSquare, endSquare;

bool start = true;
bool End = true;

struct CompareSquare {
    bool operator()(const Square& a, const Square& b) const {
        if (a.fScore == b.fScore) {
            return a.gScore > b.gScore; // Reverse order for gScore
        }
        return a.fScore > b.fScore;
    }
};

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
    //
    std::string shader_location("../res/shaders/");

    std::string used_shaders("shader");
    Shader ourShader(shader_location + used_shaders + std::string(".vert"),
                     shader_location + used_shaders + std::string(".frag"));


    std::string used_shader2("shaderStart");
    Shader startShader(shader_location + used_shader2 + std::string(".vert"),
                       shader_location + used_shader2 + std::string(".frag"));

    ////////////////////////////////////////////////////////////////////////////////////

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

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
        processInput(window);


        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // also clear the depth buffer now!

        //| GL_DEPTH_BUFFER_BIT activate shader
        ourShader.use();
        startShader.use();

        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));



        // pass transformation matrices to the shader
        ourShader.setMat4("projection",projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setMat4("view", view);

        startShader.setMat4("projection",projection);
        startShader.setMat4("view", view);

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
                startShader.setMat4("model", model);

                glm::vec4 uni;
                if(m[i][j].sType == SQUARE_TYPE::START) {
                    int st = glGetUniformLocation(startShader.ID, "colorS");
                    uni = m[i][j].color();
                    glUniform4f(st, uni.x, uni.y, uni.z, uni.w);
                    start = false;
                    startSquare = m[i][j];
                }

                if(m[i][j].sType == SQUARE_TYPE::END) {
                    int e = glGetUniformLocation(startShader.ID, "colorE");
                    uni = m[i][j].color();
                    glUniform4f(e, uni.x, uni.y, uni.z, uni.w);
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

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
        // etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

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

        if (m[x][y].sType != SQUARE_TYPE::START && m[x][y].sType != SQUARE_TYPE::END)
            m[x][y].sType = SQUARE_TYPE::BARRIER;

    }


    int startAlgorithm = glfwGetKey(window, GLFW_KEY_SPACE);
    if (startAlgorithm == GLFW_PRESS && !start && !End)
    {
        aStar();
    }
    else if (startAlgorithm == GLFW_PRESS){
        cout<<"Add start and end position"<<endl;
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

void makeGrid(::int32_t rows, ::int32_t cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Square x(i, j, SQUARE_TYPE::NORMAL);
            m[i][j] = x;
        }
    }
}
// --------------------------------------------------------------------------------------------
//                             MANHATTAN HEURISTIC FUNCTION

float heuristicFunction(const Square& m1, const Square& m2)
{
    float x1 = m1.x, y1 = m1.y;
    float x2 = m2.x, y2 = m2.y;
    return abs(x1-x2) + abs(y1-y2);
}

// ------------------------------------------------------------------------------------------
//                                       ALGORITHM
/*
bool algorithm()
{
    // fscore, cnt, start - node
    // cameFrom - keep track of the path
    // g-_score = {} - dicationary comprenhension
    // gscore(start) && fscore(start) = h(start.getPost, end.getPos)
    // openSetHash - we need to check if smth is in the priority queue

    int cnt = 0; // when we inserted the squares into the queue
//    priority_queue<std::tuple<int, int, Square*>, std::vector<std::tuple<int, int, Square*>>, Compare> openSet;

    openSet.push(std::make_tuple(0,cnt,&startSquare));

    unordered_map<Square*, Square*> cameFrom;
    unordered_map<Square*, int> gScore;
    unordered_map<Square*, int> fScore;
    unordered_set<Square*> openSetHASH;

    for(auto& row : m){
        for (auto& square: row) {
            gScore[&square] = INT_MAX;
            fScore[&square] = INT_MAX;
        }
    }

    gScore[&startSquare] = 0;
    fScore[&startSquare] = heuristicFunction(startSquare, endSquare);
    openSetHASH.insert(&startSquare);

    while(!openSetHASH.empty()){
        Square* current = get<2>(openSet.top());
        openSet.pop();
        openSetHASH.erase(current);

        if(current == &endSquare)
        {

            return true;
        }

        int i = current->x, j = current->y;
//        vector<Square> neighbours = updateNeighbours(current->x, current->y);
//        for(Square neighbour: neighbours){
//            int tmpGscore = gScore[current] + 1;
//            gScore[&neighbour] = 1000;
//
//            if(tmpGscore < gScore[&neighbour]){
//                cameFrom[&neighbour] = current;
//                gScore[&neighbour] = tmpGscore;
//                fScore[&neighbour] = tmpGscore + heuristicFunction(neighbour,endSquare);
//
//                if(openSetHASH.find(&neighbour) == openSetHASH.end()){
//                    cnt++;
//                    openSet.emplace(fScore[&neighbour],cnt,&neighbour);
//                    openSetHASH.insert(&neighbour);
//                }
//            }
//        }


        if (i < 20 && m[i+1][j].sType != SQUARE_TYPE::BARRIER)      // DOWN
        {
            int tmpGscore = gScore[current] + 1;

            if(tmpGscore < gScore[& m[i + 1][j]]){
                cameFrom[& m[i+1][j]] = current;
                gScore[& m[i+1][j]] = tmpGscore;
                fScore[& m[i+1][j]] = tmpGscore + heuristicFunction( m[i + 1][j],endSquare);

                if(openSetHASH.find(& m[i + 1][j]) == openSetHASH.end()){
                    cnt++;
                    openSet.push(std::make_tuple(fScore[& m[i + 1][j]],cnt,& m[i + 1][j]));
                    openSetHASH.insert(& m[i + 1][j]);
                    m[i + 1][j].sType = SQUARE_TYPE::OPEN;
                }
            }
        }
        if(i  > 0 && m[i-1][j].sType != SQUARE_TYPE::BARRIER)      // UP
        {
            int tmpGscore = gScore[current] + 1;

            if(tmpGscore < gScore[&m[i - 1][j]]){
                cameFrom[&m[i - 1][j]] = current;
                gScore[&m[i - 1][j]] = tmpGscore;
                fScore[&m[i - 1][j]] = tmpGscore + heuristicFunction(m[i - 1][j],endSquare);

                if(openSetHASH.find(&m[i - 1][j]) == openSetHASH.end()){
                    cnt++;
                    openSet.emplace(fScore[&m[i - 1][j]],cnt,&m[i - 1][j]);
                    openSetHASH.insert(&m[i - 1][j]);
                    m[i - 1][j].sType = SQUARE_TYPE::OPEN;
                }
            }
        }

        if (j < 20 && m[i][j+1].sType != SQUARE_TYPE::BARRIER)    // RIGHT
        {
            int tmpGscore = gScore[current] + 1;

            if(tmpGscore < gScore[&m[i][j+1]]){
                cameFrom[&m[i][j+1]] = current;
                gScore[&m[i][j+1]] = tmpGscore;
                fScore[&m[i][j+1]] = tmpGscore + heuristicFunction(m[i][j+1],endSquare);

                if(openSetHASH.find(&m[i][j+1]) == openSetHASH.end()){
                    cnt++;
                    openSet.emplace(fScore[&m[i][j+1]],cnt,&m[i][j+1]);
                    openSetHASH.insert(&m[i][j+1]);
                    m[i][j+1].sType = SQUARE_TYPE::OPEN;
                }
            }
        }

        if(j > 0 && m[i][j-1].sType != SQUARE_TYPE::BARRIER)     // LEFT
        {

            int tmpGscore = gScore[current] + 1;


            if(tmpGscore < gScore[&m[i][j-1]]){
                cameFrom[& m[i][j-1]] = current;
                gScore[& m[i][j-1]] = tmpGscore;
                fScore[& m[i][j-1]] = tmpGscore + heuristicFunction(m[i][j-1],endSquare);

                if(openSetHASH.find(& m[i][j-1]) == openSetHASH.end()){
                    cnt++;
                    openSet.emplace(fScore[& m[i][j-1]],cnt,& m[i][j-1]);
                    openSetHASH.insert(& m[i][j-1]);
                    m[i ][j-1].sType = SQUARE_TYPE::OPEN;
                }
            }
        }

        if(current != &startSquare)
            current->sType = SQUARE_TYPE::CLOSED;

    }
    return false;
}
*/

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

bool aStar(){

    bool closedList[20][20] = {false};

    m[startSquare.x][startSquare.y].fScore = heuristicFunction(startSquare, endSquare);
    m[startSquare.x][startSquare.y].gScore = 0.0;

    std::set<Square> openSet;
    openSet.insert(m[startSquare.x][startSquare.y]);

    while (!openSet.empty())
    {
        Square current = *openSet.begin();
        openSet.erase(*openSet.begin());

        ::int32_t i = current.x, j = current.y;

        if(i == endSquare.x && j == endSquare.y) {
            makePath(current.x,current.y);
            m[endSquare.x][endSquare.y].sType = SQUARE_TYPE::END;
            return true;
        }

        closedList[i][j] = true;
        float tmpGscore = current.gScore + 1.0;

        if (i < 20 && m[i + 1][j].sType != SQUARE_TYPE::BARRIER && !closedList[i + 1][j])      // DOWN
        {
            float fNew =  tmpGscore + heuristicFunction(m[i+1][j],endSquare);

            if(m[i+1][j].fScore == FLT_MAX || m[i+1][j].fScore > fNew){
                m[i+1][j].parentX = i;
                m[i+1][j].parentY = j;

                m[i+1][j].gScore = tmpGscore;
                m[i+1][j].fScore = fNew;

                openSet.insert(m[i+1][j]);
            }
        }


        if(i > 0 && m[i-1][j].sType != SQUARE_TYPE::BARRIER && !closedList[i - 1][j]){    // UP
            float fNew =  tmpGscore + heuristicFunction(m[i-1][j],endSquare);

            if(m[i-1][j].fScore == FLT_MAX || m[i-1][j].fScore > fNew)
            {
                m[i-1][j].parentX = i;
                m[i-1][j].parentY = j;

                m[i-1][j].gScore = tmpGscore;
                m[i-1][j].fScore = fNew;

                openSet.insert(m[i-1][j]);
            }
        }

        if (j < 20 && m[i][j+1].sType != SQUARE_TYPE::BARRIER && !closedList[i][j+1]){
            float fNew =  tmpGscore + heuristicFunction(m[i][j+1],endSquare);

            if(m[i][j+1].fScore == FLT_MAX || m[i][j+1].fScore > fNew) {
                m[i][j + 1].parentX = i;
                m[i][j + 1].parentY = j;

                m[i][j + 1].gScore = tmpGscore;
                m[i][j + 1].fScore = fNew;

                openSet.insert(m[i][j+1]);
            }
       }

        if(j > 0 && m[i][j-1].sType != SQUARE_TYPE::BARRIER && !closedList[i][j-1]){
            float fNew =  tmpGscore + heuristicFunction(m[i][j-1],endSquare);

            if(m[i][j-1].fScore == FLT_MAX || m[i][j-1].fScore > fNew){
                m[i][j-1].parentX = i;
                m[i][j-1].parentY = j;

                m[i][j-1].gScore = tmpGscore;
                m[i][j-1].fScore = fNew;

                openSet.insert(m[i][j-1]);
            }
        }
    }

    return false;
}
