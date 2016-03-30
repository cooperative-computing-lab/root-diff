#include "root_comparator.h"

static char *read_obj_buf (TFile *fn, TKey *k, Int_t obj_len)
{
    char *obj_buf = new char[obj_len]; 
    int offset = k->GetSeekKey();
    fn->SetOffset(offset);
    fn->ReadBuffer(obj_buf, obj_len);
    return obj_buf;

}

static bool cmp_obj_buf(const char* obj_buf_1, const char *obj_buf_2, Int_t obj_len)
{
    int i;
    for(i = 0; i<obj_len; i++) {
        
        if(obj_buf_1[i] != obj_buf_2[i]) {
            return false;
        }
    }

    return true;
}

static bool logic_compare(TKey *k_1, TKey *k_2)
{

    bool obj_len_eq, cycle_eq,
         cl_name_eq, obj_name_eq;

    k_1->GetObjlen() == k_2->GetObjlen() ?
        obj_len_eq = true : obj_len_eq = false; 
    
    k_1->GetCycle() == k_2->GetCycle() ?
        cycle_eq = true : cycle_eq = false;
    
    strcmp(k_1->GetName(), k_2->GetName()) ?
        cl_name_eq = false : cl_name_eq = true;

    strcmp(k_1->GetClassName(), k_2->GetClassName()) ?
        obj_name_eq = false : obj_name_eq = true;
    
    if (!obj_len_eq || !cycle_eq ||
            !cl_name_eq || !obj_name_eq) {
        return false;
    }
    
    return true;
}

bool Logic_comparator::root_file_cmp (char *fn1, char *fn2) 
{   
    TFile f1(fn1), f2(fn2);

    TIter next_1(f1.GetListOfKeys()),
          next_2(f2.GetListOfKeys());
 
    TKey *k_1, *k_2;

    bool is_equal = true;

    while (true) {
        k_1=(TKey*)next_1();
        k_2=(TKey*)next_2();

        // If there is no more keys
        if(!k_1 && !k_2) {
            break;
        }

        // If the size of two lists are not equal
        // then two root files are not equal
        if((k_1 && !k_2) || (!k_1 && k_2)) {
            is_equal = false;
            break;
        }

        if (!logic_compare(k_1, k_2)) {
            is_equal = false;
            break;
        }
    }

    return is_equal;
}



bool Strict_comparator::root_file_cmp (char *fn1, char *fn2)
{
    TFile f1(fn1), f2(fn2);

    TIter next_1(f1.GetListOfKeys()),
          next_2(f2.GetListOfKeys());
 
    TKey *k_1, *k_2;  

    bool is_equal = true;
   
    char *obj_buf_1 = NULL, *obj_buf_2 = NULL; 

    while(true) {
        k_1=(TKey*)next_1();
        k_2=(TKey*)next_2();

        // If there is no more keys
        if(!k_1 && !k_2) {
            break;
        }

        // If the size of two lists are not equal
        // then two root files are not equal
        if((k_1 && !k_2) || (!k_1 && k_2)) {
            is_equal = false;
            break;
        } 

        if (!logic_compare(k_1, k_2)) {
            is_equal = false;
            break;
        }
                
        // compare buffer byte by byte
        Int_t obj_len = k_1->GetObjlen(); 
        
        obj_buf_1 = read_obj_buf(&f1, k_1, obj_len);
        obj_buf_2 = read_obj_buf(&f2, k_2, obj_len);
        
        if (!cmp_obj_buf(obj_buf_1, obj_buf_2, obj_len)) {
            is_equal = false;
            break;
        }
        
    }
    
    if(obj_buf_1 != NULL) {
        delete obj_buf_1; 
    }

    if(obj_buf_2 != NULL) {
        delete obj_buf_2;
    }
    return is_equal;
}

bool Exact_comparator::root_file_cmp (char *fn1, char *fn2)
{   
    TFile f1(fn1), f2(fn2);

    TIter next_1(f1.GetListOfKeys()),
          next_2(f2.GetListOfKeys());
 
    TKey *k_1, *k_2;

    bool is_equal = true;

    char *obj_buf_1 = NULL, *obj_buf_2 = NULL; 

    while (true) {
        k_1=(TKey*)next_1();
        k_2=(TKey*)next_2();

        // If there is no more keys
        if(!k_1 && !k_2) {
            break;
        }

        // If the size of two lists are not equal
        // then two root files are not equal
        if((k_1 && !k_2) || (!k_1 && k_2)) {
            is_equal = false;
            break;
        }

        if (!logic_compare(k_1, k_2)) {
            is_equal = false;
            break;
        }

        // compare buffer byte by byte
        Int_t obj_len = k_1->GetObjlen(); 
        
        obj_buf_1 = read_obj_buf(&f1, k_1, obj_len);
        obj_buf_2 = read_obj_buf(&f2, k_2, obj_len);
        
        if (!cmp_obj_buf(obj_buf_1, obj_buf_2, obj_len)) {
            is_equal = false;
            break;
        }

        // compare the timestamp of two object
        const TDatime time_1 = k_1->GetDatime(),
              time_2 = k_2->GetDatime(); 

        if((time_1.GetDate() != time_2.GetDate()) 
                || (time_1.GetTime() != time_2.GetTime())) {
            is_equal = false;
            break;
        }

    }

    if(obj_buf_1 != NULL) {
        delete obj_buf_1; 
    }

    if(obj_buf_2 != NULL) {
        delete obj_buf_2;
    }
    return is_equal;
}
