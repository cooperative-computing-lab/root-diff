#include "root_file_comparator.h"

Obj_info *get_obj_info(char *header, long cur, const TFile *f) 
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

    TFile f_1(fn_1), f_2(fn_2);

    TIter n_1(f_1.GetListOfKeys()),
          n_2(f_2.GetListOfKeys());

    debug("There are %d objects.", f_1.GetListOfKeys()->Capacity());
    debug("There are %d objects.", f_2.GetListOfKeys()->Capacity());

    TKey *k_1, *k_2;

    bool logic_eq = true, strict_eq = true, exact_eq = true;
  
    // Create log file 
    ofstream log_f;
    if (!log_f) { 
        cout << "cannot create log file" << endl;
    }
    log_f.open(log_fn);

    Timer tmr;
    double t = tmr.elapsed();

    while(true) {
        k_1 = (TKey *)n_1();
        k_2 = (TKey *)n_2(); 

        if (!k_1 || !k_2){
            break;
        }

        debug("The %s length is %ld", k_1->GetClassName(), k_1->GetObjlen());
        debug("The %s length is %ld", k_2->GetClassName(), k_2->GetObjlen());

        if (!roc->logic_cmp(k_1, k_2)) {

            log_f << k_1->GetName() << " object of " << k_1->GetClassName() << 
                " class in " << fn_1 << " at " << k_1->GetSeekKey() 
                << " is NOT LOGICALLY EQUAL to " << k_2->GetName() << " object of " 
                << k_2->GetClassName() << " class in " << fn_2 << " at " 
                << k_2->GetSeekKey() << endl;

            logic_eq = false;
            strict_eq = false;
            exact_eq = false;
            break; 
        }

        if (!roc->strict_cmp(k_1, k_2)) {
            log_f << k_1->GetName() << " object of " << k_1->GetClassName() << 
                " class in " << fn_1 << " at " << k_1->GetSeekKey() 
                << " is NOT STRICTLY EQUAL to " << k_2->GetName() << " object of " 
                << k_2->GetClassName() << " class in " << fn_2 << " at " 
                << k_2->GetSeekKey() << endl;
            
            strict_eq = false;    
            exact_eq = false;
        }

        if (!roc->exact_cmp(k_1, k_2)) {

            log_f << k_1->GetName() << " object of " << k_1->GetClassName() << 
                " class in " << fn_1 << " at " << k_1->GetSeekKey() 
                << " and " << k_2->GetName() << " object of " 
                << k_2->GetClassName() << " class in " << fn_2 << " at " 
                << k_2->GetSeekKey() << " have different timestamp." << endl;

            exact_eq = false;
        }
    } 

    if ((!k_1 && k_2) || (k_1 && !k_2)) {
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

}
