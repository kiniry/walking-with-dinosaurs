#include "assert2.h"

void assert2(float val1, float val2, float margin){
	float val = val1-val2;
	assert(val>=(-margin) && val<=margin);
}

void assert2(int val1, int val2){

	assert(val1==val2);
}