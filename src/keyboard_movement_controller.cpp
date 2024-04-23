#include "keyboard_movement_controller.hpp"

#include <limits>

namespace pb{
    void KeyboardMovementController::moveInPlaneXZY(GLFWwindow* window, float dt, PbGameObject& gameObject){
                
        glm::vec3 rotate{0};

        //really illegal but I'm going to do mouse movement code here
        //TODO: we need to find a way to pass the current window width to here

        //this might be a bad idea cause it get's the window width every game tick
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        if(glfwGetKey(window, keys.enableOrbit) == GLFW_PRESS){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            //orbit logic
            if(firstClick){
                glfwSetCursorPos(window, width/2, height/2);
                firstClick = false;
            }

            double mouseX;
		    double mouseY;
		    glfwGetCursorPos(window, &mouseX, &mouseY);

            float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		    float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

            rotate.x -= rotX;
            rotate.y += rotY;

            glfwSetCursorPos(window, width/2, height/2);

        } else if (glfwGetKey(window, keys.enableOrbit) == GLFW_RELEASE){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		    firstClick = true;
        }

        if(glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
        if(glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
        if(glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
        if(glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

        if(glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()){
            gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
        }

        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

        float yaw = gameObject.transform.rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
        const glm::vec3 upDir{0.f, -1.f, 0.f};

        glm::vec3 moveDir{0.f};
        if(glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if(glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if(glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if(glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if(glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if(glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;
        
        if(glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()){
            gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }

    }
}

