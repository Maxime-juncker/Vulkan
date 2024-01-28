#pragma once
#include "Model.h"

#include <memory>
namespace Application
{
	struct Transform2dComponent
	{
		glm::vec2 translation{};
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() 
		{
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMat
			{
				{c, s},
				{-s, c}
			};

			glm::mat2 scaleMat
			{ 
				{scale.x, 0.0f}, 
				{0.0f, scale.y} 
			};
			return rotMat * scaleMat;
		}
	};

	class GameObject
	{
	public:
		using id_t = unsigned int;

		static GameObject CreateGameObject()
		{
			static id_t currentId = 0;
			return GameObject{ currentId++ };
		}

		id_t GetId() const { return id; }

		std::shared_ptr<Model> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

	private:
		GameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}