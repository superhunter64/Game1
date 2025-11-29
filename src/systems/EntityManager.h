#pragma once
#include <vector>
#include <string>

#include "../entities/Mob.h"
#include "../shared/Resources.h"
#include "../wireframe/Wireframe.h"

class EntityManager
{
public:
	inline Mob* SpawnMob()
	{
		auto strin = std::string("Slime");

		Mob mob(strin);
		mob.sprite.sheet = Resources::GetSheet("Sprite-0001");
		mob.sprite.anim = Resources::GetAnim("Sprite-0001", "idle");

		mob.transform.location.w = mob.sprite.frame.src.w;
		mob.transform.location.h = mob.sprite.frame.src.h;

		m_mobs.push_back(std::move(mob));

		return &m_mobs[m_mobs.size() - 1];
	};

	inline std::vector<SDL_FRect> GetRects()
	{
		std::vector<SDL_FRect> transforms = {};

		for (auto& m : m_mobs)
		{
			transforms.push_back({
				m.transform.location.x,
				m.transform.location.y,
				m.sprite.frame.src.w,
				m.sprite.frame.src.h,
			});
		}

		return transforms;
	}

	inline void Draw()
	{
		for (auto& m : m_mobs)
		{
			m.Draw();
		}
	}

private:
	std::vector<Mob> m_mobs = {};
};