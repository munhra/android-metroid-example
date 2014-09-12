/*
 * Player.cpp
 *
 *  Created on: Apr 29, 2013
 *      Author: vntraal
 */

#include "Player.h"

Player* Player::create(const char *filename) {
	Player *p = new Player();
	if (p && p->initWithFile(filename)) {
		// Set to autorelease
		p->autorelease();
		return p;
	}
	CC_SAFE_DELETE(p);
	return NULL;
}

void Player::initPlayer() {
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("playerSheet.plist");

	velocity = ccp(0.0, 0.0);
	stop = true;
	activeShoots = CCArray::create();
	defineAnimations();
	CCLog("initPlayer");
	this->setPosition(ccp(100, 450));
}

void Player::defineAnimations() {

	const char *name = "walk";
	char frameNameBuf[32] = { 0 };

	//walk animation
	CCArray *walkAnimFrames = CCArray::create();
	for (int i = 1; i <= 4; i++) {
		sprintf(frameNameBuf, "%s%d.png", name, i);
		walkAnimFrames->addObject(
				CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameNameBuf));
	}

	CCAnimation *walkAnim = CCAnimation::createWithSpriteFrames(walkAnimFrames, 0.15f);
	walkAnimation = CCRepeatForever::create(CCAnimate::create(walkAnim));
	walkAnimation->retain();

	//stop animation
	CCArray *stopAnimFrames = CCArray::create();
	for (int i = 1; i <= 1; i++) {
		sprintf(frameNameBuf, "%s%d.png", name, i);
		stopAnimFrames->addObject(
				CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameNameBuf));
	}

	CCAnimation *stopAnim = CCAnimation::createWithSpriteFrames(stopAnimFrames, 0.15f);
	stopAnimation = CCRepeatForever::create(CCAnimate::create(stopAnim));
	stopAnimation->retain();

	//jump animation
	name = "jump";

	CCArray *jumpAnimFrames = CCArray::create();
	for (int i = 1; i <= 5; i++) {
		sprintf(frameNameBuf, "%s%d.png", name, i);
		jumpAnimFrames->addObject(
				CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameNameBuf));
	}

	CCAnimation *jumpAnim = CCAnimation::createWithSpriteFrames(jumpAnimFrames, 0.15f);
	jumpAnimation = CCRepeatForever::create(CCAnimate::create(jumpAnim));
	jumpAnimation->retain();
}

CCRect Player::collisionBoundingBox() {
	CCPoint diff = ccpSub(desiredPosition, this->getPosition());
	CCRect returnBoundingBox = this->boundingBox();
	returnBoundingBox.origin.x = returnBoundingBox.origin.x + diff.x;
	returnBoundingBox.origin.y = returnBoundingBox.origin.y + diff.y;
	return returnBoundingBox;
}

CCSprite* Player::fireLaser(float deltaXView) {
	CCLog("Future Implementation");
	return NULL;
}

void Player::update(float dt) {
	CCPoint gravity = ccp(0.0, -250.0);
	CCPoint gravityStep = ccpMult(gravity, dt);
	CCPoint forwardMove = ccp(1500, 0.0);
	CCPoint backwardMove = ccp(-1500.0, 0.0);
	CCPoint forwardStep = ccpMult(forwardMove, dt); //1
	CCPoint backwardStep = ccpMult(backwardMove, dt);

	velocity = ccpAdd(velocity, gravityStep);
	velocity = ccp(velocity.x * 0.90, velocity.y); //2

	if (backwardMarch) {
		CCPoint jumpForce = ccp(0.0, 310.0);
		float jumpCutoff = 150.0;
		// The chunk of code below check if the player is jumping
		if (mightAsWellJump && onGround) {
			//jumping condition
			velocity = ccpAdd(velocity, jumpForce);
		} else if (!mightAsWellJump && velocity.y > jumpCutoff) {
			velocity = ccp(velocity.x, jumpCutoff);
		}
		velocity = ccpAdd(velocity, backwardStep);
	}

	if (forwardMarch) {
		CCPoint jumpForce = ccp(0.0, 310.0);
		float jumpCutoff = 150.0;
		// The chunk of code below check if the player is jumping
		if (mightAsWellJump && onGround) {
			//jumping condition
			velocity = ccpAdd(velocity, jumpForce);
		} else if (!mightAsWellJump && velocity.y > jumpCutoff) {
			velocity = ccp(velocity.x, jumpCutoff);
		}
		velocity = ccpAdd(velocity, forwardStep);
	}

	// limits the bondary of the speed
	CCPoint stepVelocity = ccpMult(velocity, dt);
	desiredPosition = ccpAdd(this->getPosition(), stepVelocity);

}
