#pragma once

#include "pb_model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace pb{

    struct TransformComponent{
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();
        
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
    };

    class PbGameObject {
        public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, PbGameObject>;

        static PbGameObject createGameObject() {
            static id_t currentId = 0;
            return PbGameObject{currentId++};
        }

        static PbGameObject makePointLight(float intensity = 1.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

        PbGameObject(const PbGameObject &) = delete;
        PbGameObject &operator=(const PbGameObject &) = delete;
        PbGameObject(PbGameObject &&) = default;
        PbGameObject &operator=(PbGameObject &&) = default;

        id_t getId() { return id;}

        glm::vec3 color{};
        TransformComponent transform{};

        // Optional pointer components 
        std::shared_ptr<PbModel> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

        private:
        PbGameObject(id_t objID) : id{objID} {}
            
        id_t id;
    };
}