#pragma once

#include "pb_model.h"

#include <memory>

namespace pb{

    struct Transform2dComponent{
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{{c, s}, {-s, c}};

            glm::mat2 scaleMat{{scale.x, .0f},{.0f, scale.y}};
            return rotMatrix * scaleMat;
        }
    };

    class PbGameObject {
        public:
        using id_t = unsigned int;

        static PbGameObject createGameObject() {
            static id_t currentId = 0;
            return PbGameObject{currentId++};
        }

        PbGameObject(const PbGameObject &) = delete;
        PbGameObject &operator=(const PbGameObject &) = delete;
        PbGameObject(PbGameObject &&) = default;
        PbGameObject &operator=(PbGameObject &&) = default;

        id_t getId() { return id;}

        std::shared_ptr<PbModel> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d{};

        private:
        PbGameObject(id_t objID) : id{objID} {}
            
        id_t id;
    };
}