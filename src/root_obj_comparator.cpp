#include "root_obj_comparator.h"

#define ROOT_DIR "TDirectoryFile"

static char *buffer_uncomprs(TKey *k) 
{

    //debug("unzip the buffer");

    int obj_len = k->GetObjlen(),
        key_len = k->GetKeylen(),
        offset = k->GetSeekKey() + key_len,
        nsize = k->GetNbytes(),
        comprs_len = nsize - key_len; 
 
    TFile *f = k->GetFile(); 

    char *uncomprs_buf;

    char *buf = new char[comprs_len]; 
    f->Seek(offset);
    f->ReadBuffer(buf, comprs_len); 


    if (obj_len > comprs_len) {

        // Object is compressed
        uncomprs_buf = new char[obj_len];
        int nin, nout = 0, noutot = 0, nbuf;

        while (1) {
            // Get information of compressed buffer
            R__unzip_header(&nin, (unsigned char *)buf, &nbuf);
            // Uncompress the buffer 
            R__unzip(&nin, (unsigned char *)buf, &nbuf, (unsigned char *)uncomprs_buf, &nout);
            if (!nout) { break; }
            noutot += nout;
            if (noutot >= obj_len) { break; }
            buf += nin;
            uncomprs_buf += nout;
        }

    } else {
        // Object is not compressed
        uncomprs_buf = new char[comprs_len];
        memcpy(uncomprs_buf, buf, comprs_len); 
    } 

    delete [] buf;

    return uncomprs_buf;
}

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
    
    debug("Compare the compressed buffer");   
    // Since TDirectoryFile class has fUUID attribute,
    // we could not compare two TDirectoryFile objects  
    
    if (!strcmp(k_1->GetClassName(), ROOT_DIR)) { return true; } 

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

    return (rc == 0 ? true : false); 
}


bool Uncmprs_comparator::strict_cmp(TKey *k_1, TKey *k_2)
{
    
    debug("Compare the uncompressed buffer.");
    if (strcmp(k_1->GetClassName(), ROOT_DIR) == 0) { return true; }

    char *uncomprs_buf_1 = buffer_uncomprs(k_1),
         *uncomprs_buf_2 = buffer_uncomprs(k_2);    
    
    int obj_len = k_1->GetObjlen();

    int rc = memcmp(uncomprs_buf_1, uncomprs_buf_2, obj_len);

    delete [] uncomprs_buf_1;
    delete [] uncomprs_buf_2;

    return (rc == 0 ? true : false);    

}

bool Reprod_comparator::strict_cmp(TKey *k_1, TKey *k_2) 
{
    cout << "Reprod_comparator::strict_cmp has not been implemented yet." << endl;
    return true;
}
