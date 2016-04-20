#ifndef __ROOT_OBJ_COMP_H__
#define __ROOT_OBJ_COMP_H__

#include <iostream>
#include "RZip.h"
#include "TObject.h"
#include "TFile.h"
#include "TKey.h"
#include "TBuffer.h"
#include "TBufferFile.h"
#include "dbg.h"
#include "TCollection.h"

using namespace std;

// Base class
class Rootobj_comparator
{
public:
    bool logic_cmp(TKey *k1, TKey *k2);
    bool exact_cmp(TKey *k1, TKey *k2);
    virtual bool strict_cmp(TKey *k1, TKey *k2) = 0;
};

class Cmprs_comparator : public Rootobj_comparator
{
public:
   bool strict_cmp(TKey *k1, TKey *k2); 
};

class Uncmprs_comparator : public Rootobj_comparator
{
public:
   bool strict_cmp(TKey *k1, TKey *k2); 

};

class Reprod_comparator : public Rootobj_comparator
{
public:
   bool strict_cmp(TKey *k1, TKey *k2); 
};

#endif
