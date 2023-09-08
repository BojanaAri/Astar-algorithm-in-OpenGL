#include <OpenGLPrj.hpp>

#include <GLFW/glfw3.h>

#include <Shader.hpp>
#include <Square.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

const std::string program_name = ("A* algorithm");

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void makeGrid(::uint32_t, ::uint32_t);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// variables
uint32_t nr, nc, rows = 20, cols = 20;
Square m[20][20];
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

    // build and compile our shader program
    // ------------------------------------

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
                //std::cout << i <<',' << j << ":" << xcoord <<" , " << ycoord << '\n';

                model = glm::translate(model, glm::vec3(xcoord, ycoord, 0.0f));

                ourShader.setMat4("model", model);
                startShader.setMat4("model", model);


                if(m[i][j].sType == SQUARE_TYPE::START) {
                    int st = glGetUniformLocation(startShader.ID, "colorS");
                    glm::vec4 uni = m[i][j].color();
                    glUniform4f(st, uni.x, uni.y, uni.z, uni.w);
                }

                if(m[i][j].sType == SQUARE_TYPE::END) {
                    int e = glGetUniformLocation(startShader.ID, "colorE");
                    glm::vec4 u = m[i][j].color();
                    glUniform4f(e, u.x, u.y, u.z, u.w);
                }

                int colorLocation = glGetUniformLocation(ourShader.ID, "color");
                glm::vec4 uni2 = m[i][j].color();
                glUniform4f(colorLocation, uni2.x , uni2.y, uni2.z, uni2.w);

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
    double xpos,ypos;

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if(state == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &xpos,&ypos);
        int x = (int)(xpos / 40.0f);
        int y = (int)(ypos / 40.0f);

        if(start && m[x][y].sType != SQUARE_TYPE::END) {
            m[x][y].sType = SQUARE_TYPE::START;
            start = false;
            return;
        }

        if (End &&  m[x][y].sType != SQUARE_TYPE::START)
        {
            m[x][y].sType = SQUARE_TYPE::END;
            End = false;
            return;
        }

        if( !start && !End )
         m[x][y].sType = SQUARE_TYPE::BARRIER;

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

void makeGrid(::uint32_t rows, ::uint32_t cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Square x(i, j, SQUARE_TYPE::NORMAL);
            m[i][j] = x;
        }
    }
}
