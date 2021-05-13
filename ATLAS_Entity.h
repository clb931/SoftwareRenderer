#ifndef ATLAS_ENTITY_H
#include "ATLAS_Model.h"
#include "ATLAS_Vector.h"
#include "ATLAS_Texture.h"


namespace ATLAS
{
	class Entity
	{
	public:
		Entity(Model* model, Texture* texture);
		~Entity();

		Model* getModel();
		Texture* getTexture();
		Vector3f& getPosition();
		void setPosition(Vector3f position);
		Vector3f& getRotation();
		void setRotation(Vector3f rotation);
		Vector3f& getScale();
		void setScale(Vector3f scale);
		Matrix4f getTransformationMatrix();

	private:
		Model* model;
		Texture* texture;
		Vector3f position;
		Vector3f rotation;
		Vector3f scale;
	};
}


#define ATLAS_ENTITY_H
#endif