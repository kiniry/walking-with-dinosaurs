#include <assert.h>

#define assertFloat(val1,val2,margin) assert((val1-val2)>=(-margin) && (val1-val2)<=margin);

#define assertInt(val1, val2) assert(val1==val2);