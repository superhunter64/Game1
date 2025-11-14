#include "../App.h"
#include "Animation.h"

void Animator::Update(AnimatedSprite& sprite)
{
    sprite.frameElapsed += App::DeltaTime;
    if (sprite.frameElapsed > sprite.frame.duration)
    {
        sprite.frameElapsed = 0;
        sprite.animFrame += 1;
        if (sprite.animFrame > sprite.anim.to)
        {
            sprite.animFrame = sprite.anim.from;
        }

        sprite.frame = sprite.sheet->sprites.at(sprite.animFrame);
    }
}

void Animator::Play(AnimatedSprite& sprite, const std::string& animName)
{
    if(sprite.sheet->animations.find(animName) != sprite.sheet->animations.end())
    {
        sprite.anim = sprite.sheet->animations[animName];
        sprite.animFrame = sprite.anim.to;

        sprite.frame = sprite.sheet->sprites.at(sprite.animFrame);
        sprite.frameElapsed = 0;
    }
    else
    {
        SDL_Log("Could not find animation: %s", animName.c_str());
    }
}
