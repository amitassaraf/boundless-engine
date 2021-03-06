#include "perspective_camera.hpp"
#include "core/events/key_event.hpp"
#include "core/events/mouse_event.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

namespace Boundless {

    PerspectiveCamera::PerspectiveCamera(EventManager& eventManager) : Layer(eventManager, "WindowLayer") {
        m_position = glm::vec3(0.0f, 0.0f,  0.0f);
        m_front    = glm::vec3(0.0f, 0.0f, -1.0f);
        m_up       = glm::vec3(0.0f, 1.0f,  0.0f);
        m_lastX = 400;
        m_lastY = 320;

        m_projectionMatrix = glm::perspective(glm::radians(45.0f), (float)800/(float)640, 0.1f, 100.0f);

        m_eventManager.appendListener(EventType::KEY_PRESSED, [&](const Ref<Event> event) {
            Ref<KeyPressedEvent> keyPressEvent = std::dynamic_pointer_cast<KeyPressedEvent> (event);
            
            if (keyPressEvent->getKeyCode() == 87) { // W
                m_heldKeys |= 1 << 0;
            } else if (keyPressEvent->getKeyCode() == 83) { // S
                m_heldKeys |= 1 << 1;
            } else if (keyPressEvent->getKeyCode() == 65) { // A
                m_heldKeys |= 1 << 2;
            } else if (keyPressEvent->getKeyCode() == 68) { // D
                m_heldKeys |= 1 << 3;
            }
        });

        m_eventManager.appendListener(EventType::KEY_RELEASED, [&](const Ref<Event> event) {
            Ref<KeyReleasedEvent> keyReleasedEvent = std::dynamic_pointer_cast<KeyReleasedEvent> (event);
            
            if (keyReleasedEvent->getKeyCode() == 87) { // W
                m_heldKeys ^= 1 << 0;
            } else if (keyReleasedEvent->getKeyCode() == 83) { // S
                m_heldKeys ^= 1 << 1;
            } else if (keyReleasedEvent->getKeyCode() == 65) { // A
                m_heldKeys ^= 1 << 2;
            } else if (keyReleasedEvent->getKeyCode() == 68) { // D
                m_heldKeys ^= 1 << 3;
            }
        });

        m_eventManager.appendListener(EventType::MOUSE_MOVED, [&](const Ref<Event> event) {
            Ref<MouseMovedEvent> mouseMovedEvent = std::dynamic_pointer_cast<MouseMovedEvent> (event);

            if (firstMouse) // initially set to true
            {
                m_lastX = mouseMovedEvent->getX();
                m_lastY = mouseMovedEvent->getY();
                firstMouse = false;
            }
            float xoffset = mouseMovedEvent->getX() - m_lastX;
            float yoffset = m_lastY - mouseMovedEvent->getY(); // reversed since y-coordinates range from bottom to top
            m_lastX = mouseMovedEvent->getX();
            m_lastY = mouseMovedEvent->getY();

            const float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw   += xoffset;
            pitch += yoffset;

            if(pitch > 89.0f)
                pitch =  89.0f;
            if(pitch < -89.0f)
                pitch = -89.0f;  
            
            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            m_front = glm::normalize(direction);
            glm::vec3 right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f,  0.0f)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            m_up    = glm::normalize(glm::cross(right, m_front));
            calculateView();
        });


    }
    PerspectiveCamera::~PerspectiveCamera() {}

    void PerspectiveCamera::onAttach() {

    }

    void PerspectiveCamera::onDetach() {

    }

    void PerspectiveCamera::onUpdate() {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        float cameraSpeed = m_speed * deltaTime;
        if ((m_heldKeys & 1) == 1) { // W
            moveForward(cameraSpeed);
        }
        if (((m_heldKeys >> 1) & 1) == 1) { // S
            moveBackward(cameraSpeed);
        }
        if (((m_heldKeys >> 2) & 1) == 1) { // A
            moveLeft(cameraSpeed);
        }
        if (((m_heldKeys >> 3) & 1) == 1) { // D
            moveRight(cameraSpeed);
        }
    }

    void PerspectiveCamera::calculateView() {
        m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
    }

    void PerspectiveCamera::moveForward(float speed) {
        m_position += speed * m_front;
        calculateView();
    }

    void PerspectiveCamera::moveBackward(float speed) {
        m_position -= speed * m_front;
        calculateView();
    }

    void PerspectiveCamera::moveLeft(float speed) {
        m_position -= glm::normalize(glm::cross(m_front, m_up)) * speed;
        calculateView();
    }

    void PerspectiveCamera::moveRight(float speed) {
        m_position += glm::normalize(glm::cross(m_front, m_up)) * speed;
        calculateView();
    }

}