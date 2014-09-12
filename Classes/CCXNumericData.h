/*
 * CCXNumericData.h
 *
 *  Created on: May 3, 2013
 *      Author: vntraal
 */

#ifndef CCXNUMERICDATA_H_
#define CCXNUMERICDATA_H_

#include "cocos2d.h"

using namespace cocos2d;

class CCXNumericData: public CCObject {

public:
	CCPoint point;
	float floatValue;
	int intValue;

	void setPoint(CCPoint pnt);
	CCPoint getPoint();

	void setFloatValue(float value);
	float getFloatValue();

	void setIntValue(int value);
	int getIntValue();


};

#endif /* CCXNUMERICDATA_H_ */
