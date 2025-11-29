#pragma once
#include <vector>

#include "../entities/Mob.h"
#include "../shared/Resources.h"

class EntityManager
{
public:
	inline Mob* SpawnMob(std::string_view mobName = "")
	{
		Mob mob = {};
		mob.name = "Slime";
		mob.sprite.sheet = Resources::GetSheet("Sprite-0001");
		mob.sprite.anim = Resources::GetAnim("Sprite-0001", "idle");

		m_mobs.push_back(mob);

		return &mob;
	};

	inline std::vector<Transform2D*> GetTransforms()
	{
		std::vector<Transform2D*> transforms = {};

		for (auto& m : m_mobs)
		{
			transforms.push_back(&m.transform);
		}

		return transforms;
	}


private:
	std::vector<Mob> m_mobs = {};
};