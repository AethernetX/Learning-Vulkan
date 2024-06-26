#pragma once

#include "pb_game_object.h"
#include "pb_window.h"

namespace pb{
    class KeyboardMovementController {
        public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_SHIFT;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int enableOrbit = GLFW_KEY_LEFT_CONTROL;
        };

        //mouse look code
        bool firstClick = false;
        float sensitivity = 100.0f;

        void moveInPlaneXZY(GLFWwindow* window, float dt, PbGameObject& gameObject);

        KeyMappings keys{};
        float moveSpeed{3.f};
        float lookSpeed{1.5f};
    };
}