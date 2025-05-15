#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bits/stdc++.h>

#include "./header/Object.h"
#include "./header/stb_image.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int createShader(const string &filename, const string &type);
unsigned int createProgram(unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader);
unsigned int modelVAO(Object &model);
unsigned int loadTexture(const char *tFileName);
void init();

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// Shader
unsigned int vertexShader, geometryShader, fragmentShader, shaderProgram;

// Texture
unsigned int airplaneTexture, creeperTexture;

// VAO, VBO
unsigned int  airplaneVAO, cubeVAO, creeperVAO;

// Objects
Object  *airplaneObject, *cubeObject, *creeperObject;

const int airplaneHeight = 36;
const int rotateCreeperSpeed = 30;
const int rotateAirplaneSpeed = 90;

float rotateCreeperDegree = 0;
float rotateAirplaneDegree = 0;
float rotateHelicopterDegree = 0;

int rotateAxisDegree = 180;

bool useHelicopter = false;
bool explotion = false; 
bool isFlashing = false;
bool isDithering = false;
bool isBurning = false;
float creeperScale = 1.0f;
bool isScaling = false;
float explotiontime;
bool isExist = false;
bool is4 = false;
glm::vec3 heli_body1Color = glm::vec3 (250, 250, 0);
glm::vec3 heli_body2Color = glm::vec3 (255, 255, 255);
glm::vec3 heli_body3Color = glm::vec3 (140, 70, 20);
glm::vec3 heli_connectColor = glm::vec3 (255, 255, 255);
glm::vec3 heli_bla1Color = glm::vec3 (255, 182, 193);
glm::vec3 heli_bla2Color = glm::vec3 (240, 255, 240);
glm::vec3 heli_bla3Color = glm::vec3 (255, 250, 205);
glm::vec3 heli_bla4Color = glm::vec3 (255, 218, 185);


int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Final Project - Group 26", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    init();

    // Enable depth test, face culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Set viewport
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // render loop variables
    double dt;
    double lastTime = glfwGetTime();
    double currentTime;

    // 獲取 uniform 變數位置
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    int heliColorLoc = glGetUniformLocation(shaderProgram, "heliColor");
    int useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");


////////////////////////////////////////////////////////////////////////////////////

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // render
        glClearColor(0 / 255.0, 0 / 255.0, 0 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 50.0f, 90.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        glm::mat4 base(1.0f),   airplaneModel(1.0f), creeperModel(1.0f), 
                                heli_body1Model(1.0f), heli_body2Model(1.0f), heli_body3Model(1.0f), 
                                heli_connectModel(1.0f), 
                                heli_bla1Model(1.0f), heli_bla2Model(1.0f), heli_bla3Model(1.0f), heli_bla4Model(1.0f);

        unsigned int vertexCount;


        // 設置 Shader 的 uniform
        int enableEffectLoc1 = glGetUniformLocation(shaderProgram, "enableExplotion");
        int enableFlashLoc = glGetUniformLocation(shaderProgram, "enableFlash");
        int enableDitherLoc = glGetUniformLocation(shaderProgram, "enableDither");
        int enableBurningLoc = glGetUniformLocation(shaderProgram, "enableBurning");
        int timeLoc = glGetUniformLocation(shaderProgram, "time");
        glUniform1i(enableEffectLoc1, explotion); 
        glUniform1i(enableFlashLoc, isFlashing); 
        glUniform1i(enableDitherLoc, isDithering);
        glUniform1i(enableBurningLoc, isBurning);
        float currentTime = glfwGetTime();
        glUniform1f(timeLoc, currentTime-explotiontime); 

        const float targetScale = 6.5f;
        const float scaleSpeed = 3.0f;
        if (isScaling && creeperScale < targetScale) {
            creeperScale += scaleSpeed * dt;
            if (creeperScale >= targetScale) {
                creeperScale = targetScale;
                isScaling = false; 
            }
        }


        glm::vec3 rotateAxis(-1.0f, 0.0f, 0.0f);
        rotateAxis = glm::rotate(base, glm::radians(static_cast<float>(rotateAxisDegree)), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(rotateAxis, 1.0f);
        airplaneModel = glm::rotate(base, glm::radians(rotateAirplaneDegree), rotateAxis);
        airplaneModel = glm::translate(airplaneModel, glm::vec3(0.0f, airplaneHeight, 0.0f));
        airplaneModel = glm::rotate(airplaneModel, glm::radians(static_cast<float>(rotateAxisDegree)), glm::vec3(0.0f, 1.0f, 0.0f));

        if(!useHelicopter){
            vertexCount = airplaneObject->positions.size() / 3;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(airplaneModel));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, airplaneTexture);
            glBindVertexArray(airplaneVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);  //unbind
        }
        if(isExist){
            vertexCount = creeperObject->positions.size() / 3;

            creeperModel = glm::rotate(base, glm::radians(rotateCreeperDegree), glm::vec3(0.0f, 1.0f, 0.0f));
            creeperModel = glm::scale(creeperModel, glm::vec3(creeperScale, creeperScale, creeperScale));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(creeperModel));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, creeperTexture);
            glBindVertexArray(creeperVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        }

        ///////////////////////////////////
        if(useHelicopter){
            //body1
            vertexCount = cubeObject->positions.size() / 3;
            heli_body1Model = glm::translate(airplaneModel, glm::vec3(0, 0, 0));
            heli_body1Model = glm::scale(heli_body1Model, glm::vec3(10.0f, 12.0f, 5.0f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(heli_body1Model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 0);
            glUniform3fv(heliColorLoc, 1, glm::value_ptr(heli_body1Color));

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);


            //body2
            vertexCount = cubeObject->positions.size() / 3;
            heli_body2Model = glm::translate(airplaneModel, glm::vec3(0, -2, 0));
            heli_body2Model = glm::scale(heli_body2Model, glm::vec3(10.2f, 2.2f, 5.2f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(heli_body2Model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 0);
            glUniform3fv(heliColorLoc, 1, glm::value_ptr(heli_body2Color));

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);

            //body3
            vertexCount = cubeObject->positions.size() / 3;
            heli_body3Model = glm::translate(airplaneModel, glm::vec3(0, -4.35, 0));
            heli_body3Model = glm::scale(heli_body3Model, glm::vec3(10.4f, 3.4f, 5.4f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(heli_body3Model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 0);
            glUniform3fv(heliColorLoc, 1, glm::value_ptr(heli_body3Color));

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);

            //connecter
            vertexCount = cubeObject->positions.size() / 3;
            heli_connectModel = glm::translate(airplaneModel, glm::vec3(0, 7, 0));
            heli_connectModel = glm::scale(heli_connectModel, glm::vec3(1.5f, 2.0f, 1.5f));
            heli_connectModel = glm::rotate(heli_connectModel, glm::radians(rotateHelicopterDegree), glm::vec3(0.0f, 1.0f, 0.0f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(heli_connectModel));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 0);
            glUniform3fv(heliColorLoc, 1, glm::value_ptr(heli_connectColor));

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);

            //bla1
            vertexCount = cubeObject->positions.size() / 3;
            heli_bla1Model = glm::translate(heli_connectModel, glm::vec3(1.0, 0.35f, 0.0f));
            heli_bla1Model = glm::scale(heli_bla1Model, glm::vec3(3.0f, 0.2f, 1.0f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(heli_bla1Model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 0);
            glUniform3fv(heliColorLoc, 1, glm::value_ptr(heli_bla1Color));

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);

            //bla2
            vertexCount = cubeObject->positions.size() / 3;
            heli_bla2Model = glm::translate(heli_connectModel, glm::vec3(-1.0, 0.35f, 0.0f));
            heli_bla2Model = glm::scale(heli_bla2Model, glm::vec3(3.0f, 0.2f, 1.0f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(heli_bla2Model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 0);
            glUniform3fv(heliColorLoc, 1, glm::value_ptr(heli_bla2Color));

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);

            //bla3
            vertexCount = cubeObject->positions.size() / 3;
            heli_bla3Model = glm::translate(heli_connectModel, glm::vec3(0.0, 0.35f, 1.0f));
            heli_bla3Model = glm::scale(heli_bla3Model, glm::vec3(1.0f, 0.2f, 3.0f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(heli_bla3Model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 0);
            glUniform3fv(heliColorLoc, 1, glm::value_ptr(heli_bla3Color));

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);

            //bla4
            vertexCount = cubeObject->positions.size() / 3;
            heli_bla4Model = glm::translate(heli_connectModel, glm::vec3(0.0, 0.35f, -1.0f));
            heli_bla4Model = glm::scale(heli_bla4Model, glm::vec3(1.0f, 0.2f, 3.0f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(heli_bla4Model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(useTextureLoc, 0);
            glUniform3fv(heliColorLoc, 1, glm::value_ptr(heli_bla4Color));

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);

            glBindVertexArray(0);

        }

////////////////////////////////////////////////////////////////
        // Status update
        currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        // 更新旋轉角度
        rotateCreeperDegree += rotateCreeperSpeed * dt;
        rotateAirplaneDegree += rotateAirplaneSpeed * dt;
        rotateHelicopterDegree += 5;

        if(is4) rotateAxisDegree += 1;
        
///////////////////////////////////////////////////////////////////////////////////

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

unsigned int createShader(const string &filename, const string &type) {
    // 讀取文件內容
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceStr = buffer.str();
    const char* source = sourceStr.c_str();

    // 創建 Shader 物件
    GLenum shaderType;
    if(type == "vert")shaderType = GL_VERTEX_SHADER;
    else if(type == "geom") shaderType = GL_GEOMETRY_SHADER;
    else shaderType = GL_FRAGMENT_SHADER;
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    return shader;
}


unsigned int createProgram(unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader) {
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, geometryShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glDetachShader(program, vertexShader);
    glDetachShader(program, geometryShader);
    glDetachShader(program, fragmentShader);

    return program;
}

unsigned int modelVAO(Object &model) {
    unsigned int VAO, VBO[3];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);

    // 綁定 VAO
    glBindVertexArray(VAO);

    // 頂點位置
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(float), &model.positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 法線
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(float), &model.normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // 紋理座標
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, model.texcoords.size() * sizeof(float), &model.texcoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // 解綁
    return VAO;

}

unsigned int loadTexture(const string &filename) {

    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 設置紋理參數
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加載圖像
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_D:
                rotateAxisDegree += 1;
                break;
            case GLFW_KEY_A:
                rotateAxisDegree -= 1;
                break;
            case GLFW_KEY_H:
                useHelicopter = !useHelicopter;
                break;
            case GLFW_KEY_E:
                explotiontime = glfwGetTime();
                explotion = !explotion;
                break;
            case GLFW_KEY_F:
                isFlashing = !isFlashing;
                break;
            case GLFW_KEY_B:
                isScaling = true;
                break;
            case GLFW_KEY_1:
                isDithering = !isDithering;
                break;
            case GLFW_KEY_2:
                isBurning = !isBurning;
                break;
            case GLFW_KEY_3:
                isExist = !isExist;
                break;
            case GLFW_KEY_4:
                is4 = !is4;
                break;
        }
    }
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void init() {
#if defined(__linux__) || defined(__APPLE__)
    string dirShader = "../../src/shaders/";
    string dirAsset = "../../src/asset/obj/";
    string dirTexture = "../../src/asset/texture/";
#else
    string dirShader = "..\\..\\src\\shaders\\";
    string dirAsset = "..\\..\\src\\asset\\obj\\";
    string dirTexture = "..\\..\\src\\asset\\texture\\";
#endif
    // Object
    airplaneObject = new Object(dirAsset + "airplane.obj");
    cubeObject = new Object(dirAsset + "cube.obj");
    creeperObject = new Object(dirAsset + "creeper.obj");

    // Shader
    vertexShader = createShader(dirShader + "vertexShader.vert", "vert");
    geometryShader = createShader(dirShader + "geometryShader.geom", "geom");
    fragmentShader = createShader(dirShader + "fragmentShader.frag", "frag");
    shaderProgram = createProgram(vertexShader, geometryShader, fragmentShader);
    glUseProgram(shaderProgram);

    // Texture
    airplaneTexture = loadTexture(dirTexture + "airplane.jpg");
    creeperTexture = loadTexture(dirTexture + "creeper.png");

    // VAO, VBO
    airplaneVAO = modelVAO(*airplaneObject);
    cubeVAO = modelVAO(*cubeObject);
    creeperVAO = modelVAO(*creeperObject);
}
