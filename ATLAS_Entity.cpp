#include "stdafx.h"
#include "ATLAS_Entity.h"


namespace ATLAS {
    Entity::Entity(Model* model, Texture* texture)
        : model(model), texture(texture), position(), rotation(), scale(1.0f, 1.0f, 1.0f)
    {}

    Entity::~Entity() {

    }
    
    Model* Entity::getModel() {
        return model;
    }

    Texture* Entity::getTexture() {
        return texture;
    }

    Vector3f& Entity::getPosition() {
        return position;
    }
    
    void Entity::setPosition(Vector3f position) {
        this->position = position;
    }

    Vector3f& Entity::getRotation() {
        return rotation;
    }
    
    void Entity::setRotation(Vector3f rotation) {
        this->rotation = rotation;
    }

    Vector3f& Entity::getScale() {
        return scale;
    }
    
    void Entity::setScale(Vector3f scale) {
        this->scale = scale;
    }

    Matrix4f Entity::getTransformationMatrix() {
        return TranslationMatrix4f(position.x, position.y, position.z)
            * RotationMatrix4f(rotation.x, rotation.y, rotation.z)
            * ScaleMatrix4f(scale.x, scale.y, scale.z);
    }
}
