//
//  load_hero.cpp
//  test_3
//
//  Created by GAOXiaoXu on 6/6/16.
//  Copyright © 2016 GAOXiaoXu. All rights reserved.
//
#include <vector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <GLUT/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "objloader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "bullet.hpp"
#include "load_hero.hpp"

//GLFWwindow* window;



void load_h(const char * objName, GLFWwindow * window){

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    
    GLuint MatrixID2 = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID2 = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID2 = glGetUniformLocation(programID, "M");
    
    // Load the texture
    //	GLuint Texture = loadDDS("uvmap.DDS");
    GLuint Texture = loadBMP_custom("hero.bmp");
    
    
    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    
    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> vertices2;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec2> uvs2;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> normals2;

    bool res2 = loadOBJ("hero.obj", vertices, uvs, normals);
    bool res1 = loadOBJ("sphere.obj", vertices2, uvs2, normals2);
    
    
    // Load it into a VBO - hero
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    
    GLuint vertexbuffer2;
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);
    
    GLuint uvbuffer2;
    glGenBuffers(1, &uvbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
    glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0], GL_STATIC_DRAW);
    
    GLuint normalbuffer2;
    glGenBuffers(1, &normalbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
    glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(glm::vec3), &normals2[0], GL_STATIC_DRAW);
    
 
    
    
    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    
    do{
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use our shader
        glUseProgram(programID);
        
        // Compute the MVP matrix from keyboard and mouse input - hero
        

        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        

        glm::vec3 lightPos = glm::vec3(4,2,4);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              0,                  // attribute
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
                              1,                                // attribute
                              2,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
                              2,                                // attribute
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() );
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        

        
        // Swap buffers
//        glfwSwapBuffers(window);
//        glfwPollEvents();
        
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
                bullet();

        
                glm::mat4 ProjectionMatrix2 = getProjectionMatrix2();
                glm::mat4 ViewMatrix2 = getViewMatrix2();
                glm::mat4 ModelMatrix2 = glm::mat4(10);
                glm::mat4 MVP2 = ProjectionMatrix2 * ViewMatrix2 * ModelMatrix2;
        
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP2[0][0]);
                glUniformMatrix4fv(ModelMatrixID2, 1, GL_FALSE, &ModelMatrix2[0][0]);
                glUniformMatrix4fv(ViewMatrixID2, 1, GL_FALSE, &ViewMatrix2[0][0]);
        
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
        glVertexAttribPointer(
                              0,                  // attribute
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
        glVertexAttribPointer(
                              1,                                // attribute
                              2,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
        glVertexAttribPointer(
                              2,                                // attribute
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, vertices2.size() );
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        
        
        
        
        
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
        
        
        
        
        
        
        
        
        
        

        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO and shader
//    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &vertexbuffer2);

    

//    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &uvbuffer2);


//    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &normalbuffer2);


    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);
}


