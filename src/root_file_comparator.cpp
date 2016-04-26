#include "root_file_comparator.h"

using namespace std;

Obj_info *get_obj_info(const char *header, long cur, const TFile *f) 
{
    unsigned int datime;
    Obj_info *obj_info = new Obj_info();
    check_mem(obj_info, "cannot allocate memory space for Obj_info class");

    frombuf(header, &(obj_info->nbytes));
    if (!obj_info->nbytes) {
        log_err("The size of the object buffer is unaccessible.");
    } 

    Version_t version_key;
    frombuf(header, &(obj_info->version_key));
    frombuf(header, &(obj_info->obj_len));   
    frombuf(header, &(obj_info->datime)); 
    frombuf(header, &(obj_info->key_len));   
    frombuf(header, &(obj_info->cycle));   

    if (version_key > 1000) {
        //for large file the type of seek_key and seek_pdir is long
        frombuf(header, &seek_key);
        frombuf(header, &seek_pdir);
    } else {
        int s_key, s_dir;
        frombuf(header, &s_key);
        obj->seek_key = (long)s_key; 
        obj->seek_pdir = (long)s_dir;
    }

    // Get the class name of object
    char class_name_len;
    frombuf(header, &class_name_len);
    for (int i=0; i<class_name_len; i++) {
        frombuf(header, &(obj_info->class_name[i]);
    }
    obj_info->class_name[(int)(class_name_len)] = '\0';

    if (cur == f->GetSeekFree()) {
        strlcpy(obj_info->class_name, "FreeSegments", NAME_LEN);
    }
    if (cur == f->GetSeekInfo()) {
        strlcpy(obj_info->class_name, "StreamerInfo", NAME_LEN);
    }
    if (cur == f->GetSeekKeys()) {
        strlcpy(obj_info->class_name, "KeysList", NAME_LEN);
    }

    TDatime::GetDateTime(datime, obj_info->date, obj_info->time);
    return obj_info;

error:
    if(obj_info) {
        delete obj_info;
    }
    return NULL;
}

Agree_lv Rootfile_comparator::root_file_cmp(char *fn_1, char *fn_2, 
        const char *mode, const char *log_fn) 
{
    Rootobj_comparator *roc;   

    if (!strcmp(mode, "CC")) {
        roc = new Cmprs_comparator();
    } else if (!strcmp(mode, "UC")) {
        roc = new Uncmprs_comparator();
    } else {
        cout << "unknown option" <<  mode << endl; 
        exit(1);
    } 

    if (access(fn_1, F_OK) == -1) {
        cout << fn_1 << " does not exist." << endl;
        exit(1);
    }

    if (access(fn_2, F_OK) == -1) {
        cout << fn_2 << " does not exist." << endl;
        exit(1);
    }

    TFile *f_1 = new TFile(fn_1); 
    TFile *f_2 = new TFile(fn_2);

    bool logic_eq = true, strict_eq = true, exact_eq = true;
  
    // Create log file 
    ofstream log_f;
    if (!log_f) { 
        cout << "cannot create log file" << endl;
    }
    log_f.open(log_fn);

    Timer tmr;
    double t = tmr.elapsed();

    int nread_1 = 64,
        nread_2 = 64;

    long cur_1 = HEADER_LEN, 
         cur_2 = HEADER_LEN, 
         f1_end = f_1->GetEND(), 
         f2_end = f_2->GetEND();

    char header_1[HEADER_LEN],
         header_2[HEADER_LEN];

    Obj_info *obj_info_1,
             *obj_info_2;

    while(cur_1 < f1_end && cur_2 < f2_end) {
        f_1->Seek(cur_1);
        f_2->Seek(cur_2);
        
        if (cur_1 + nread_1 >= f1_end) {
            nread_1 = f1_end - cur_1 - 1;
        }
        if (cur_2 + nread_2 >= f2_end) {
            nread_2 = f2_end - cur_2 - 1;
        }

        if (f_1->ReadBuffer(header_1, nread_1)) {
            log_err("Failed to read the object header from %s from disk at %ld", fn_1, cur_1);
        } 
        if (f_2->ReadBuffer(header_2, nread_2)) {
            log_err("Failed to read the object header from %s from disk at %ld", fn_2, cur_2);
        }

        obj_info_1 = this.get_obj_info(header_1, cur_1, f_1);
        obj_info_2 = this.get_obj_info(header_2, cur_2, f_2);

        if (!roc->logic_cmp(obj_info_1, obj_info_2)) {

            log_f << " Instance of " << obj_info_1->class_name << 
                " in "<< fn_1 << " at " << cur_1
                << " is NOT LOGICALLY EQUAL to " << " Instance of " 
                << obj_info_2->class_name << " class in " << fn_2 << " at " 
                << cur_2 << endl;

            logic_eq = false;
            strict_eq = false;
            exact_eq = false;
            break; 
        }

        if (!roc->strict_cmp(obj_info_1, f_1, obj_info_2, f_2)) {
            
            log_f << " Instance of " << obj_info_1->class_name << 
                " in "<< fn_1 << " at " << cur_1
                << " is NOT STRICTLY EQUAL to " << " Instance of " 
                << obj_info_2->class_name << " class in " << fn_2 << " at " 
                << cur_2 << endl;

            strict_eq = false;    
            exact_eq = false;
        }

        if (!roc->exact_cmp(obj_info_1, obj_info_2)) {

            log_f << " Instance of " << obj_info_1->class_name << 
                " in "<< fn_1 << " at " << cur_1
                << " is NOT EXACTLY EQUAL to " << " Instance of " 
                << obj_info_2->class_name << " class in " << fn_2 << " at " 
                << cur_2 << endl;

            exact_eq = false;
        }
    } 

    if ((cur_1 >= f1_end && cur_2 < f2_end) || 
            (cur_1 < f1_end && cur_2 >= f2_end)) {
        log_f << "Number of objects in " << fn_1 << " and " << fn_2 << " are not equal."<< endl;
        log_f.close();
        delete roc; 
        return Not_eq;
    }

    delete roc;

    tmr.reset();
    t = tmr.elapsed();
    log_f << endl; 
    log_f << "Comparison took: " << t << endl;

    log_f.close();

    if (!logic_eq && !strict_eq && !exact_eq){
        return Not_eq;
    } else if (exact_eq){
        return Exact_eq;
    } else if (strict_eq){
        return Strict_eq;
    } else {
        return Logic_eq;
    }

error:
    exit(1);

}
