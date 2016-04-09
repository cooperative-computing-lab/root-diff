#include "root_obj_comparator.h"

bool Rootobj_comparator::logic_cmp(TKey *k_1, TKey *k_2)
{

    if (k_1->GetObjlen() != k_2->GetObjlen()) {
        return false;
    }
    
    if (k_1->GetCycle() != k_2->GetCycle()) {
        return false;
    }
    
    if (strcmp(k_1->GetName(), k_2->GetName()) != 0) {
        return false;
    }

    if (strcmp(k_1->GetClassName(), k_2->GetClassName()) != 0) {
        return false;
    }
    
    return true;
}

bool Rootobj_comparator::exact_cmp(TKey *k_1, TKey *k_2) 
{
    return (k_1->GetDatime() == k_2->GetDatime());
}

bool Cmprs_comparator::strict_cmp(TKey *k_1, TKey *k_2) 
{
    TFile *f_1 = k_1->GetFile(),
          *f_2 = k_2->GetFile();

    int nsize_1 = k_1->GetNbytes(),
        nsize_2 = k_2->GetNbytes();

    int k_len_1 = k_1->GetKeylen(),
        k_len_2 = k_2->GetKeylen();

    int cmprs_len_1 = nsize_1 - k_len_1,
        cmprs_len_2 = nsize_2 - k_len_2;

    int offset_1 = k_1->GetSeekKey() + k_len_1,
        offset_2 = k_2->GetSeekKey() + k_len_2;

    char *buf_1 = new char[cmprs_len_1],
         *buf_2 = new char[cmprs_len_2];
   
    f_1->Seek(offset_1);
    f_1->ReadBuffer(buf_1, cmprs_len_1);

    f_2->Seek(offset_2);
    f_2->ReadBuffer(buf_2, cmprs_len_2);
   
    int rc = memcmp(buf_1, buf_2, cmprs_len_1);

    delete [] buf_1;
    delete [] buf_2;

    if (rc == 0) {
        return true;
    } else {
        return false;
    }

}

bool Uncmprs_comparator::strict_cmp(TKey *k_1, TKey *k_2)
{
    return true;
}

bool Reprod_comparator::strict_cmp(TKey *k_1, TKey *k_2) 
{
    return true;
}
