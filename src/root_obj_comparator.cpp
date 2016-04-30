#include "root_obj_comparator.h"
#include "dbg.h"

#define ROOT_DIR "TDirectoryFile"

using namespace std;

/*

header = header_array; * Uncompress the object buffer exclude the key buffer 
 */

static unsigned char *buffer_uncomprs(Obj_info *obj_info, TFile *f) 
{
    if (debug_mode) {
        debug("unzip the buffer");
    }

    int obj_len = obj_info->obj_len,
        key_len = obj_info->key_len,
        nsize = obj_info->nbytes,
        comprs_len = nsize - key_len; 
 
    long offset = obj_info->seek_key + key_len;

    unsigned char *uncomprs_buf;

    char *buf = new char[comprs_len]; 

    f->Seek(offset);
    f->ReadBuffer(buf, comprs_len); 

    if (obj_len > comprs_len) {

        // Object is compressed
        uncomprs_buf = new unsigned char[obj_len];
        int nin, nout = 0, noutot = 0, nbuf;

        while (1) {
            // Get information of compressed buffer
            R__unzip_header(&nin, (unsigned char*)buf, &nbuf);
            // Uncompress the buffer 
            R__unzip(&nin, (unsigned char*)buf, &nbuf, uncomprs_buf, &nout);
            if (!nout) { break; }
            noutot += nout;
            if (noutot >= obj_len) { break; }
            buf += nin;
            uncomprs_buf += nout;
        }

    } else {
        // Object is not compressed
        uncomprs_buf = new unsigned char[comprs_len];
        memcpy(uncomprs_buf, buf, comprs_len); 
    } 

    delete [] buf;

    return uncomprs_buf;
}

/*
 * If two objects have same object length, number of cycles, class name and 
 * object name, then they are logically equal to each other.
 */

bool Rootobj_comparator::logic_cmp(Obj_info *obj_info_1, Obj_info *obj_info_2)
{

    if ((obj_info_1->obj_len) != (obj_info_2->obj_len)) {
        return false;
    }

    if (obj_info_1->cycle != obj_info_2->cycle) {
        return false;
    }
    
    if (strcmp(obj_info_1->class_name, obj_info_2->class_name) != 0) {
        return false;
    }
  
    return true;
}

/*
 * If two objects are logically and strictly equal to each other, then
 * they are exactlly equal if they have same timestamp.
 */

bool Rootobj_comparator::exact_cmp(Obj_info *obj_info_1, Obj_info *obj_info_2) 
{
    if (obj_info_1->date != obj_info_2->date) {
        return false;
    }

    if (obj_info_1->time != obj_info_2->time) {
        return false;
    }  

    return true;
}

bool Cmprs_comparator::strict_cmp(Obj_info *obj_info_1, TFile *f_1, Obj_info *obj_info_2, TFile *f_2) 
{

    if(debug_mode) {
        debug("Compare the compressed buffer of %s object in file 1 and %s object in file 2", obj_info_1->class_name, obj_info_2->class_name);   
    }

    // Since TDirectoryFile class has fUUID attribute,
    // we could not compare two TDirectoryFile objects  
    
    if (!strcmp(obj_info_1->class_name, ROOT_DIR)) { return true; } 
    if (!strcmp(obj_info_2->class_name, ROOT_DIR)) { return true; } 

    int nsize_1 = obj_info_1->nbytes,
        nsize_2 = obj_info_2->nbytes;

    int k_len_1 = obj_info_1->key_len,
        k_len_2 = obj_info_2->key_len;

    int cmprs_len_1 = nsize_1 - k_len_1,
        cmprs_len_2 = nsize_2 - k_len_2;

    long offset_1 = obj_info_1->seek_key + k_len_1,
         offset_2 = obj_info_2->seek_key + k_len_2;

    char *buf_1 = new char[cmprs_len_1],
         *buf_2 = new char[cmprs_len_2];
  
    f_1->Seek(offset_1);
    f_1->ReadBuffer(buf_1, cmprs_len_1);

    f_2->Seek(offset_2);
    f_2->ReadBuffer(buf_2, cmprs_len_2);
   
    int rc = memcmp((unsigned char*)buf_1, (unsigned char*)buf_2, cmprs_len_1);

    delete [] buf_1;
    delete [] buf_2;

    return (rc == 0 ? true : false); 
}


bool Uncmprs_comparator::strict_cmp(Obj_info *obj_info_1, TFile *f1, Obj_info *obj_info_2, TFile *f2)
{
    
    if(debug_mode) {
        debug("Compare the uncompressed buffer of %s object in file 1 and %s object file 2", obj_info_1->class_name, obj_info_2->class_name);   
    }

    if (strcmp(obj_info_1->class_name, ROOT_DIR) == 0) { return true; }
    if (strcmp(obj_info_2->class_name, ROOT_DIR) == 0) { return true; }

    unsigned char *uncomprs_buf_1 = buffer_uncomprs(obj_info_1, f1),
                  *uncomprs_buf_2 = buffer_uncomprs(obj_info_2, f2);    
    
    int obj_len = obj_info_1->obj_len;

    int rc = memcmp(uncomprs_buf_1, uncomprs_buf_2, obj_len);

    delete [] uncomprs_buf_1;
    delete [] uncomprs_buf_2;

    return (rc == 0 ? true : false);    

}
