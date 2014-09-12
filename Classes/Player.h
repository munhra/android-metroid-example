#ifndef PLAYER_H_
#define PLAYER_H_

#include "cocos2d.h"

using namespace cocos2d;

class Player: public CCSprite {

public:
	CCPoint velocity;
	CCPoint desiredPosition;
	bool onGround;
	bool forwardMarch;
	bool backwardMarch;
	bool shoot;
	bool stop;
	bool mightAsWellJump;
	CCAction *walkAnimation;
	CCAction *stopAnimation;
	CCAction *jumpAnimation;
	CCAction *deathAnimation;
	CCArray *activeShoots;

	static Player* create(const char *filename);
	CCRect collisionBoundingBox();
	CCSprite* fireLaser(float deltaXView);
	void update(float dt);
	void initPlayer();

private:
	void defineAnimations();

};
#endif /*PLAYER_H_*/
