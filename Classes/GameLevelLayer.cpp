#include "GameLevelLayer.h"
//#include "SimpleAudioEngine.h"
#include "Player.h"
#include "CCXNumericData.h"

using namespace cocos2d;
//using namespace CocosDenshion;

CCTMXTiledMap *map;
CCTMXLayer *walls;
CCTMXLayer *hazards;
bool isGameOver;
bool touchMoved;
float beginYTouch;
float deltaYTouch;
float beginXTouch;
float stopPosition;

CCPoint actualPosition;

CCScene* GameLevel::scene() {
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	GameLevel *layer = GameLevel::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void GameLevel::setViewpointCenter(CCPoint position) {
	// it is very related to the size of the tilemap
	// it is necessary to parametrize this point
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	int x = fmax(position.x, winSize.width / 2);
	int y = fmax(position.y, winSize.height / 2);
	x = fmin(x, (map->getMapSize().width * map->getTileSize().width) - winSize.width / 2);
	//y = fmin(y, (map->getMapSize().height * map->getTileSize().height) - winSize.height / 2);
	y = fmin(y, (480) - winSize.height / 2);
	actualPosition = ccp(x, y);
	CCPoint centerOfView = ccp(winSize.width / 2, winSize.height / 2);
	CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
	map->setPosition(viewPoint);
}

void GameLevel::update(float dt) {

	if (!isGameOver) {
		this->player->update(dt);
		this->checkForAndResolveCollisions(this->player);
		this->setViewpointCenter(this->player->getPosition());
		//this->handleHazardCollisions(this->player);
		this->checkToStopPlayer(player);
		this->checkForWin();
	}
}

void GameLevel::checkForWin() {
	if (this->player->getPosition().x > 3130.0) {
		this->gameOver(true);
	}
}

bool GameLevel::init() {
	CCLOG("Init Scene");
	if (!CCLayer::init()) {
		return false;
	}

	isGameOver = false;

	if (isGameOver) {
		CCLOG("Game over error");
	} else {
		CCLOG("Game over ok");
	}

	CCLOG("Set blue sky");
	CCLayerColor *blueSky = CCLayerColor::create(ccc4(100, 100, 250, 255));
	this->addChild(blueSky);

	map = CCTMXTiledMap::create("level1.tmx");
	this->addChild(map);

	walls = map->layerNamed("walls");
	hazards = map->layerNamed("hazards");

	this->player = Player::create("walk1.png");
	this->player->initPlayer();
	//CCLOG("Init method replaceTime %d \n", replaceTime);

	map->addChild(this->player, 15);

	stopPosition = this->player->getPosition().x;

	this->schedule(schedule_selector(GameLevel::update));
	this->setTouchEnabled(true);

	return true;
}

CCPoint tileCoordForPosition(CCPoint position) {

	float x = floor(position.x / map->getTileSize().width);
	float levelHeightInPixels = map->getMapSize().height * map->getTileSize().height;
	float y = floor((levelHeightInPixels - position.y) / map->getTileSize().height);
	return ccp(x, y);
}

CCRect tileRectFromTileCoords(CCPoint tileCoords) {
	float levelHeightInPixels = map->getMapSize().height * map->getTileSize().height;
	CCPoint origin = ccp(tileCoords.x * map->getTileSize().width,
			levelHeightInPixels - ((tileCoords.y + 1) * map->getTileSize().height));
	return CCRectMake(origin.x, origin.y, map->getTileSize().width, map->getTileSize().height);
}

void GameLevel::reloadGamePlayScene(CCObject *sender) {
	//remove the scene from parent, this is not made there is a problem
	//when replacing the scene
	this->removeFromParentAndCleanup(true);
	CCDirector::sharedDirector()->replaceScene(
			CCTransitionZoomFlipX::create(0.5, GameLevel::scene()));
}

void GameLevel::gameOver(bool won) {

	isGameOver = true;
	const char *gameText;

	if (won == true) {
		gameText = "You Won!";
		CCLOG("Game over you won ");
	} else {
		gameText = "You have Died!";
		CCLOG("Game over you died ");
	}

	CCLabelTTF *diedLabel = CCLabelTTF::create(gameText, "Mark Felt", 40);
	diedLabel->setPosition(ccp(240, 200));

	CCMoveBy *slideIn = CCMoveBy::create(1, ccp(0, 250));
	CCMenuItemImage *replay = CCMenuItemImage::create("replay.png", "replay.png", "replay.png",
			this, menu_selector(GameLevel::reloadGamePlayScene));

	CCArray *menuItems = CCArray::create();
	menuItems->addObject(replay);
	CCMenu *menu = CCMenu::createWithArray(menuItems);
	menu->setPosition(ccp(240, -100));
	this->addChild(menu);
	this->addChild(diedLabel);
	menu->runAction(slideIn);
}

CCRect CCRectIntersection(CCRect r1, CCRect r2) {

	CCRect intersection;
	intersection = CCRectMake(std::max(r1.getMinX(), r2.getMinX()),
			std::max(r1.getMinY(), r2.getMinY()), 0, 0);
	intersection.size.width = std::min(r1.getMaxX(), r2.getMaxX()) - intersection.getMinX();
	intersection.size.height = std::min(r1.getMaxY(), r2.getMaxY()) - intersection.getMinY();

	return intersection;
}

CCArray* GameLevel::getSurroundingTilesAtPosition(CCPoint position, CCTMXLayer *layer) {

	CCPoint plPos = tileCoordForPosition(position);
	CCArray *gids = CCArray::create();

	for (int i = 0; i < 9; i++) {
		int c = i % 3;
		int r = (int) (i / 3);
		CCPoint tilePos = ccp(plPos.x + (c - 1), plPos.y + (r - 1));

		if (tilePos.y > (map->getMapSize().height - 1)) {
			this->gameOver(false);
			return NULL;
		}

		int tgid = layer->tileGIDAt(tilePos);

		CCRect tileRect = tileRectFromTileCoords(tilePos);

		CCDictionary *tileDict = CCDictionary::create();

		CCXNumericData *tilePosData = new CCXNumericData();
		CCXNumericData *tgidData = new CCXNumericData();
		CCXNumericData *rectOrgXData = new CCXNumericData();
		CCXNumericData *rectOrgYData = new CCXNumericData();

		tilePosData->setPoint(tilePos);
		tgidData->setIntValue(tgid);
		rectOrgXData->setFloatValue(tileRect.origin.x);
		rectOrgYData->setFloatValue(tileRect.origin.y);
		tileDict->setObject(tgidData, "gid");
		tileDict->setObject(rectOrgXData, "x");
		tileDict->setObject(rectOrgYData, "y");
		tileDict->setObject(tilePosData, "tilePos");

		gids->addObject(tileDict);
	}

	gids->removeObjectAtIndex(4);
	gids->insertObject(gids->objectAtIndex(2), 6);
	gids->removeObjectAtIndex(2);
	gids->exchangeObjectAtIndex(4, 6);
	gids->exchangeObjectAtIndex(0, 4);

	return gids;
}

void GameLevel::checkForAndResolveCollisions(Player *p) {

	CCArray *tiles = getSurroundingTilesAtPosition(p->getPosition(), walls);

	if (isGameOver) {
		return;
	}

	p->onGround = false;

	for (int tileIndx = 0; tileIndx < tiles->count(); tileIndx++) {

		CCRect pRect = p->collisionBoundingBox();
		CCDictionary *dic = (CCDictionary *) tiles->objectAtIndex(tileIndx);
		CCXNumericData *tgidData = (CCXNumericData *) dic->objectForKey("gid");
		if (tgidData->getIntValue() > 0) {

			CCXNumericData *rectOrgXData = (CCXNumericData *) dic->objectForKey("x");
			CCXNumericData *rectOrgYData = (CCXNumericData *) dic->objectForKey("y");

			CCRect tileRect = CCRectMake(rectOrgXData->getFloatValue(),
					rectOrgYData->getFloatValue(), map->getTileSize().width,
					map->getTileSize().height);

			// the colision is not detected.
			if (p->collisionBoundingBox().intersectsRect(tileRect)) {
				CCRect intersection = CCRectIntersection(pRect, tileRect);
				if (tileIndx == 0) {
					//tile is directly below player
					p->desiredPosition = ccp(p->desiredPosition.x,
							p->desiredPosition.y + intersection.size.height);
					p->velocity = ccp(p->velocity.x, 0.0);
					p->onGround = true;
				} else if (tileIndx == 1) {
					//tile is directly above player
					p->desiredPosition = ccp(p->desiredPosition.x,
							p->desiredPosition.y - intersection.size.height);
					p->velocity = ccp(p->velocity.x, 0.0);
				} else if (tileIndx == 2) {
					//tile is left of player
					p->desiredPosition = ccp(p->desiredPosition.x + intersection.size.width,
							p->desiredPosition.y);
				} else if (tileIndx == 3) {
					//tile is right of player
					p->desiredPosition = ccp(p->desiredPosition.x - intersection.size.width,
							p->desiredPosition.y);
				} else {
					if (intersection.size.width > intersection.size.height) {
						//tile is diagonal, but resolving collision vertially
						p->velocity = ccp(p->velocity.x, 0.0); //////Here
						float resolutionHeight;
						if (tileIndx > 5) {
							resolutionHeight = intersection.size.height;
							p->onGround = true; //////Here
						} else {
							resolutionHeight = -intersection.size.height;
						}
						p->desiredPosition = ccp(p->desiredPosition.x,
								p->desiredPosition.y + resolutionHeight);

					} else {
						float resolutionWidth;
						if (tileIndx == 6 || tileIndx == 4) {
							resolutionWidth = intersection.size.width;
						} else {
							resolutionWidth = -intersection.size.width;
						}
						p->desiredPosition = ccp(p->desiredPosition.x + resolutionWidth,
								p->desiredPosition.y);
					}
				}
			}
		}
	}
	p->setPosition(p->desiredPosition);
}

void GameLevel::checkToStopPlayer(Player *p) {

	if (p->forwardMarch) {
		if (p->getPosition().x >= stopPosition) {
			p->forwardMarch = false;
			p->backwardMarch = false;
			p->velocity = ccp(0, 0);
			p->stopAllActions();
			p->runAction(p->stopAnimation);
		}
	}
	if (p->backwardMarch) {
		if (p->getPosition().x <= stopPosition) {
			p->forwardMarch = false;
			p->backwardMarch = false;
			p->velocity = ccp(0, 0);
			p->stopAllActions();
			p->runAction(p->stopAnimation);
		}
	}

}

void GameLevel::handleHazardCollisions(Player *p) {

	// every position in tiles is a dictionary
	CCArray *tiles = getSurroundingTilesAtPosition(p->getPosition(), hazards);
	CCDictionary *dic = CCDictionary::create();

	CCXNumericData *tgidData = new CCXNumericData();
	CCXNumericData *rectOrgXData = new CCXNumericData();
	CCXNumericData *rectOrgYData = new CCXNumericData();

	for (int i = 0; i < tiles->count(); i++) {
		dic = (CCDictionary *) tiles->objectAtIndex(i);
		rectOrgXData = (CCXNumericData *) dic->objectForKey("x");
		rectOrgYData = (CCXNumericData *) dic->objectForKey("y");
		tgidData = (CCXNumericData *) dic->objectForKey("gid");

		CCRect tileRect = CCRectMake(rectOrgXData->getFloatValue(), rectOrgYData->getFloatValue(),
				map->getTileSize().width, map->getTileSize().height);

		CCRect pRect = p->collisionBoundingBox();

		// check if it is realy getIntValue() or why this value is checked.

		if (tgidData->getIntValue() && pRect.intersectsRect(tileRect)) {
			gameOver(false);
		}
	}
}

void GameLevel::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {
	CCLOG("%s \n", "ccTouchesEnded");

	CCSetIterator it;
	CCTouch* touch;
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	for (it = pTouches->begin(); it != pTouches->end(); it++) {
		touch = (CCTouch*) (*it);

		if (touchMoved) {
			float deltaXView = actualPosition.x - (winSize.width / 2);
			CCPoint touchLocation = this->convertTouchToNodeSpace(touch);

			if (touchLocation.x + deltaXView > this->player->getPosition().x) {
				this->player->forwardMarch = true;
				this->player->backwardMarch = false;
				this->player->setFlipX(false);
			} else if (touchLocation.x + deltaXView < this->player->getPosition().x) {
				this->player->backwardMarch = true;
				this->player->forwardMarch = false;
				this->player->setFlipX(true);
			}

			this->player->stopAllActions();
			if (deltaYTouch > 50) {
				this->player->mightAsWellJump = true;
				this->player->runAction(this->player->jumpAnimation);
			} else {
				this->player->mightAsWellJump = false;
				this->player->runAction(this->player->walkAnimation);
			}

			deltaYTouch = 0;

			float deltaXTouch = fabs(beginXTouch - touchLocation.x);


			if (deltaXTouch <= 100) {
				stopPosition = touchLocation.x + deltaXView;
			} else {
				stopPosition = this->player->getPosition().x + 100;
			}

		}
	}
	touchMoved = false;

}

void GameLevel::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {


	CCSetIterator it;
	CCTouch* touch;

	for (it = pTouches->begin(); it != pTouches->end(); it++) {
		touch = (CCTouch*) (*it);
		CCPoint touchLocation = this->convertTouchToNodeSpace(touch);
		deltaYTouch = touchLocation.y - beginYTouch;
		touchMoved = true;

	}

}

void GameLevel::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent) {


}

void GameLevel::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {

	CCSetIterator it;
	CCTouch* touch;

	for (it = pTouches->begin(); it != pTouches->end(); it++) {
		touch = (CCTouch*) (*it);
		CCPoint touchLocation = this->convertTouchToNodeSpace(touch);
		beginYTouch = touchLocation.y;
		beginXTouch = touchLocation.x;
		touchMoved = false;
	}

}

void GameLevel::menuCloseCallback(CCObject* pSender) {
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
