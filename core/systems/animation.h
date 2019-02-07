#pragma once

#include "common.h"
#include "ecs.h"
#include "systems/renderable.h"

enum AnimationType { SCALE, ALPHA };
struct CAnimated : public Component<CAnimated> {
  AnimationType animationType;
  float startValue;
  float endValue;
  float rate = 0.01;
  bool reverses;
  bool repeats;
};

// TODO: Animation curve, this animation system is currently restricted to
// linear animations
class SAnimation : public System
{
public:
  SAnimation() : System()
  {
    System::addComponentType(CAnimated::id);
    System::addComponentType(CSprite::id);
  }

  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CAnimated* animation = (CAnimated*)components[0];
    CSprite* sprite = (CSprite*)components[1];

    switch (animation->animationType) {
      // Scale animation
    case SCALE: {
      const float start = animation->startValue;
      const float newScale =
          lerp(sprite->getScale(), animation->endValue, animation->rate);
      const float scaleDiff = newScale - sprite->getScale();
      const float width = sprite->getWidth(), height = sprite->getHeight();
      sprite->setScale(newScale);

      float diffX = (width / 2) * scaleDiff, diffY = (height / 2) * scaleDiff;

      sprite->setPosition(sprite->getX() - diffX / 2,
                          sprite->getY() - diffY / 2);

      if (abs(sprite->getScale() - animation->endValue) < 0.01) {
        // Animation has ended
        if (animation->reverses) {
          animation->startValue = animation->endValue;
          animation->endValue = start;
        } else if (animation->repeats)
          sprite->setScale(animation->startValue);
      }
    } break;
    // Alpha animation
    case ALPHA: {
      const float newAlpha =
          lerp(sprite->alpha, animation->endValue, animation->rate);
      sprite->alpha = newAlpha;
      if (abs(sprite->alpha - animation->endValue) < 0.01) {
        if (animation->reverses) {
          float start = animation->startValue;
          animation->startValue = animation->endValue;
          animation->endValue = start;
        } else if (animation->repeats) {
          sprite->alpha = animation->startValue;
        }
      }

    } break;
    }
  }
};