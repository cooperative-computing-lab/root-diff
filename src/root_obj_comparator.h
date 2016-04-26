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

/*
 * Struct storing the object information
 */
typedef struct Obj_info {

    int key_len, cycle
        nbytes, date,
        time, obj_len;
    
    long seek_key, seek_pdir;

    char class_name[NAME_LEN],
         obj_name[NAME_LEN];

} Obj_info;


// Base class
class Rootobj_comparator
{
public:
    bool logic_cmp(Obj_info *obj_info_1, Obj_info *obj_info_1);
    bool exact_cmp(Obj_info *obj_info_1, Obj_info *obj_info_1);
    virtual bool strict_cmp(Obj_info *obj_info_1, TFile *f1, Obj_info *obj_info_1, TFile *f2) = 0;
};

class Cmprs_comparator : public Rootobj_comparator
{
public:
   bool strict_cmp(Obj_info *obj_info_1, TFile *f1, Obj_info *obj_info_1, TFile *f2); 
};

class Uncmprs_comparator : public Rootobj_comparator
{
public:
   bool strict_cmp(Obj_info *obj_info_1, TFile *f1, Obj_info *obj_info_1, TFile *f2); 

};

#endif
