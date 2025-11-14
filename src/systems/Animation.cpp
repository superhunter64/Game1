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
