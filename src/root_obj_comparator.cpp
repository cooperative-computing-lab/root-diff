#include "root_obj_comparator.h"

#define ROOT_DIR "TDirectoryFile"

/*
 * Uncompress the object buffer exclude the key buffer 
 */

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

/*
 * If two objects have same object length, number of cycles, class name and 
 * object name, then they are logically equal to each other.
 */

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

/*
 * If two objects are logically and strictly equal to each other, then
 * they are exactlly equal if they have same timestamp.
 */

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
    TObject *obj_1 = k_1->ReadObj();
    TObject *obj_2 = k_2->ReadObj();

    // As suggested in TDirectoryFile.cxx, we set the buffer size as 10000
    const int buf_size = 10000; 
    TBufferFile buf_f_1(TBuffer::kWrite, buf_size);
    TBufferFile buf_f_2(TBuffer::kWrite, buf_size);
  
    buf_f_1.MapObject(obj_1);
    buf_f_2.MapObject(obj_2);
   
    // Streaming objects to buffers 
    {
        bool is_ref_1 = obj_1->TestBit(kIsReferenced);
        bool is_ref_2 = obj_2->TestBit(kIsReferenced);

        obj_1->ResetBit(kIsReferenced);
        obj_2->ResetBit(kIsReferenced);

        obj_1->Streamer(buf_f_1);
        obj_2->Streamer(buf_f_2);

        if(is_ref_1) {
            obj_1->SetBit(kIsReferenced);
        }
        
        if(is_ref_2) {
            obj_2->SetBit(kIsReferenced);
        } 

    }

    buf_f_1.SetReadMode();
    buf_f_1.SetBufferOffset(0); 

    buf_f_2.SetReadMode();
    buf_f_2.SetBufferOffset(0); 
    char *obj_buf_1 = buf_f_1.Buffer() + k_1->GetKeylen();
    char *obj_buf_2 = buf_f_2.Buffer() + k_2->GetKeylen();

    int obj_len = k_1->GetObjlen();

    if (!memcmp(obj_buf_1, obj_buf_2, obj_len)) {
        return true;
    } else {
        return false;
    }

}
