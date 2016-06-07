// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "bullet.hpp"

glm::mat4 ViewMatrix2;
glm::mat4 ViewMatrix_rotate2;
glm::mat4 ProjectionMatrix2;

glm::mat4 getViewMatrix2(){
    return ViewMatrix_rotate2;
}
glm::mat4 getProjectionMatrix2(){
    return ProjectionMatrix2;
}

// Initial position : on +Z
glm::vec3 position2 = glm::vec3( 0, 1, 3 );

// Initial horizontal angle : toward -Z
float horizontalAngle2 = 3.14f;
// Initial vertical angle : none
float verticalAngle2 = 0.0f;
// Initial Field of View
float initialFoV2 = 45.0f;

float speed2 = 3.0f; // 3 units / second
float mouseSpeed2 = 0.005f;

float rotation2 = 0.f;




void bullet(){
    

    
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime2 = glfwGetTime();
    
    // Compute time difference between current and last frame
    double currentTime2 = glfwGetTime();
    float deltaTime2 = float(currentTime2 - lastTime2);
    
    // Get mouse position
    //	double xpos, ypos;
    //	glfwGetCursorPos(window, &xpos, &ypos);
    
    // Reset mouse position for next frame
    glfwSetCursorPos(window, 1024/2, 768/2);
    
    // Compute new orientation
    //	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
    //	verticalAngle   += mouseSpeed * float( 768/2 - ypos );
    
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction2(
                        cos(verticalAngle2) * sin(horizontalAngle2),
                        sin(verticalAngle2),
                        cos(verticalAngle2) * cos(horizontalAngle2)
                        );
    
    // Right vector
    glm::vec3 right2 = glm::vec3(sin(horizontalAngle2 - 3.14f/2.0f),0,cos(horizontalAngle2 - 3.14f/2.0f)
                                );
    
    
    
    // Up vector
    glm::vec3 up2 = glm::cross( right2, direction2 );
    
    // Move forward
    //	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
    //		position += direction * deltaTime * speed;
    //	}
    //	// Move backward
    //	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
    //		position -= direction * deltaTime * speed;
    //	}
    
    // rotate
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
//        rotation2 += 1.4f * deltaTime2 * speed2;
//    }
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
//        rotation2 -= 1.4f * deltaTime2 * speed2;
//    }
    
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position2 += right2 * deltaTime2 * speed2;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position2 -= right2 * deltaTime2 * speed2;
    }
    
    float FoV2 = initialFoV2;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
    
    // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix2 = glm::perspective(FoV2, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    ViewMatrix2       = glm::lookAt(
                                   position2,           // Camera is here
                                   position2+direction2, // and looks here : at the same position, plus "direction"
                                   up2                  // Head is up (set to 0,-1,0 to look upside-down)
                                   );
    ViewMatrix_rotate2 = glm::rotate(ViewMatrix2,rotation2,glm::vec3(0,1,0));
    
    
    // For the next frame, the "last time" will be "now"
    lastTime2 = currentTime2;
}



