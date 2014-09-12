/*
 * GameLevelLayer.h
 *
 *  Created on: Apr 29, 2013
 *      Author: vntraal
 */

#ifndef GAMELEVELLAYER_H_
#define GAMELEVELLAYER_H_

#include "cocos2d.h"
#include "Player.h"

using namespace cocos2d;

class GameLevel : public cocos2d::CCLayer
{

public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();

    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(GameLevel);

    virtual void ccTouchesMoved (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesCancelled (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesBegan (CCSet *pTouches, CCEvent *pEvent);

private:

    Player *player;

	void gameOver(bool won);
	void handleHazardCollisions(Player *p);
	void checkForAndResolveCollisions(Player *p);
	CCArray* getSurroundingTilesAtPosition(CCPoint position, CCTMXLayer *layer);
	void reloadGamePlayScene(CCObject *sender);
	void update(float dt);
	void setViewpointCenter(CCPoint position);
	void checkToStopPlayer(Player *p);
	void checkForWin();

};

#endif // GAMELEVELLAYER_H_
