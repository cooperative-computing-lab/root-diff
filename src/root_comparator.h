#ifndef __ROOT_COMP_H__
#define __ROOT_COMP_H__

#include <iostream>
#include "TFile.h"
#include "TKey.h"
#include "TBuffer.h"
#include "TCollection.h"

using namespace std;

// Base class
class Root_comparator
{
public:
    virtual bool root_file_cmp (char *fn1, char *fn2) = 0; 
};

// Derived classes

/* If two file has same hierachical structure and 
 * the objects from f1 have the same size of corresponding
 * objects in f2, then return true.
 *
 * The time stamp can be different.
 */
class Logic_comparator: public Root_comparator
{
public:
    bool root_file_cmp (char *fn1, char *fn2);
};

/* If two file has same hierachical structure and 
 * the objects from f1 are equal to the corresponding
 * objects in f2 on bit level. 
 * 
 * The time stamp can be different.
 */
class Strict_comparator: public Root_comparator
{
public:
    bool root_file_cmp (char *fn1, char *fn2);
};

/* If two file has same hierachical structure and 
 * the objects from f1 are equal to the corresponding
 * objects in f2 on bit level. 
 * 
 * The time stamp can't be different.
 */

class Exact_comparator: public Root_comparator
{
public:
    bool root_file_cmp (char *fn1, char *fn2);
};

#endif
