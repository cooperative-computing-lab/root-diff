#ifndef __ROOT_OBJ_COMP_H__
#define __ROOT_OBJ_COMP_H__

#include <iostream>
#include "RZip.h"
#include "TObject.h"
#include "TFile.h"
#include "TKey.h"
#include "TBuffer.h"
#include "TBufferFile.h"
#include "TCollection.h"
#include "dbg.h"

#define NAME_LEN 512

/*
 * Struct storing the object information
 */
typedef struct Obj_info {

    Short_t key_len,
            cycle;

    Int_t nbytes, date,
          time, obj_len;
    
    Long64_t seek_key, seek_pdir;

    char class_name[NAME_LEN];

} Obj_info;


// Base class
class Rootobj_comparator
{
public:
    bool logic_cmp(Obj_info *obj_info_1, Obj_info *obj_info_2);
    bool exact_cmp(Obj_info *obj_info_1, Obj_info *obj_info_2);
    virtual bool strict_cmp(Obj_info *obj_info_1, TFile *f1, Obj_info *obj_info_2, TFile *f2) = 0;
};

// Compressed comparator
class Cmprs_comparator : public Rootobj_comparator
{
public:
   bool strict_cmp(Obj_info *obj_info_1, TFile *f1, Obj_info *obj_info_2, TFile *f2); 
};

// Uncompressed comparator
class Uncmprs_comparator : public Rootobj_comparator
{
public:
   bool strict_cmp(Obj_info *obj_info_1, TFile *f1, Obj_info *obj_info_2, TFile *f2); 

};

#endif
