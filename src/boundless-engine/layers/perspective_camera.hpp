#pragma once
#ifndef BD_PERSPECTIVE_CAMERA_HPP_
#define BD_PERSPECTIVE_CAMERA_HPP_

#include <glm/glm.hpp>
#include "core/core.hpp"
#include "core/event_manager.hpp"
#include "logging/logger.hpp"
#include "core/layer.hpp"

namespace Boundless {
    
    class PerspectiveCamera : public Layer {
        public:
            PerspectiveCamera(EventManager& eventManager, int width, int height);
            virtual ~PerspectiveCamera();

            void onAttach() override;
            void onDetach() override;
            void onUpdate() override;
 
            inline void setPosition(const glm::vec3& position) { m_position = position; }
            inline void setFront(const glm::vec3& front) { m_front = front; }
            inline void setUp(const glm::vec3& up) { m_up = up; }

            inline float getSpeed() const { return m_speed; }
            inline const glm::vec3& getFront() const { return m_front; }
            inline const glm::vec3& getPosition() const { return m_position; }
            inline const glm::vec3& getUp() const { return m_up; }
            inline const glm::mat4& getViewProjectionMatrix() const { return m_viewProjectionMatrix; }
            inline const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
            inline const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }

            void calculateView();
            void moveForward(float speed);
            void moveBackward(float speed);
            void moveLeft(float speed);
            void moveRight(float speed);
        private:
            glm::mat4 m_projectionMatrix;
            glm::mat4 m_viewMatrix;
            glm::mat4 m_viewProjectionMatrix;

            glm::vec3 m_front;
            glm::vec3 m_position;
            glm::vec3 m_up;
            
            const float m_speed = 600.0f; // adjust accordingly
            float yaw;
            float pitch;
            float m_lastX = 400, m_lastY = 320;
            bool firstMouse = true;
            uint8_t m_heldKeys;

            float deltaTime = 0.0f;	// Time between current frame and last frame
            float lastFrame = 0.0f; // Time of last frame
    };

}


#endif // !BD_PERSPECTIVE_CAMERA_HPP_
