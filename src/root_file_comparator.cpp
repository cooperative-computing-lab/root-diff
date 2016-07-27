#include "root_file_comparator.h"

using namespace std;

/*
 * Get object information from the header (i.e. TKey)
 */

static Obj_info *get_obj_info(char *header_array, Long64_t cur, const TFile *f) 
{
    UInt_t datime;
    Obj_info *obj_info = new Obj_info();
    char *header;
    char class_name_len;
    char obj_name_len;

    header = header_array;
    frombuf(header, &(obj_info->nbytes));
    if (!obj_info->nbytes) {
        log_err("The size of the object buffer is unaccessible.");
    } 

    Version_t version_key;
    frombuf(header, &version_key);
    frombuf(header, &(obj_info->obj_len));  
    frombuf(header, &datime); 
    frombuf(header, &(obj_info->key_len));   
    frombuf(header, &(obj_info->cycle));   

    if (version_key > 1000) {
        //for large file the type of seek_key and seek_pdir is long
        frombuf(header, &(obj_info->seek_key));
        frombuf(header, &(obj_info->seek_pdir));
    } else {
        Int_t s_key, s_dir;
        frombuf(header, &s_key);
        frombuf(header, &s_dir);
        obj_info->seek_key = (Long64_t)s_key; 
        obj_info->seek_pdir = (Long64_t)s_dir;
    }

    // Get the class name of object
    frombuf(header, &class_name_len);
    for (int i=0; i<class_name_len; i++) {
        frombuf(header, &(obj_info->class_name[i]));
    }

    obj_info->class_name[int(class_name_len)] = '\0';

    
    if (cur == f->GetSeekFree()) {
        strlcpy(obj_info->class_name, "FreeSegments", NAME_LEN);
    }
    if (cur == f->GetSeekInfo()) {
        strlcpy(obj_info->class_name, "StreamerInfo", NAME_LEN);
    }
    if (cur == f->GetSeekKeys()) {
        strlcpy(obj_info->class_name, "KeysList", NAME_LEN);
    }

    frombuf(header, &obj_name_len);
    for (int i=0; i<obj_name_len; i++) {
        frombuf(header, &(obj_info->obj_name[i]));
    }

    obj_info->obj_name[int(obj_name_len)] = '\0';

    obj_info->date = 0;
    obj_info->time = 0;

    if (debug_mode) {
        debug("============%s obj info=============", obj_info->class_name);
        debug("name: %s", obj_info->obj_name);
        debug("class: %s", obj_info->class_name);
        debug("seek_key: %ld", obj_info->seek_key); 
        debug("version: %d", version_key);
        debug("nbytes: %d", obj_info->nbytes);
        debug("object len: %d", obj_info->obj_len);
        debug("datime: %d", datime);
        debug("key len: %d", obj_info->key_len);
        debug("# of cycles: %d", obj_info->cycle); 
        debug("====================================");
        cout << endl;
    }

    TDatime::GetDateTime(datime, obj_info->date, obj_info->time);
    return obj_info;

error:
    if(obj_info) {
        delete obj_info;
    }

    exit(1);
}

Agree_lv Rootfile_comparator::root_file_cmp(char *fn_1, char *fn_2, 
        const char *mode, const char *log_fn, 
        set<string> ignored_classes) 
{
    Rootobj_comparator *roc;   
    
    // Get comparison mode
    if (!strcmp(mode, "CC")) {
        roc = new Cmprs_comparator();
    } else if (!strcmp(mode, "UC")) {
        roc = new Uncmprs_comparator();
    } else {
        cout << "unknown option" <<  mode << endl; 
        exit(1);
    } 

    // Check if input files are accessible
    if (access(fn_1, F_OK) == -1) {
        cout << fn_1 << " does not exist." << endl;
        exit(1);
    }

    if (access(fn_2, F_OK) == -1) {
        cout << fn_2 << " does not exist." << endl;
        exit(1);
    }

    bool logic_eq = true, strict_eq = true, exact_eq = true;
  
    // Create log file
    ofstream log_f;
    if (!log_f) { 
        cout << "cannot create log file" << endl;
    }
    log_f.open(log_fn);

    Timer tmr;
    double t = tmr.elapsed();

    // Scan file 1 and generate object information array

    TFile *f_1 = new TFile(fn_1); 

    Int_t nwheader;
    nwheader = 64;
    Int_t nread_1 = nwheader;

    Long64_t cur_1 = HEADER_LEN, 
             f1_end = f_1->GetEND();

    char header_1[HEADER_LEN];

    Obj_info *obj_info_1;

    string class_name_str_1;

    vector<Obj_info*> objs_info; 

    while(cur_1 < f1_end) {
        f_1->Seek(cur_1);
        
        if (cur_1 + nread_1 >= f1_end) {
            nread_1 = f1_end - cur_1 - 1;
        }

        if (f_1->ReadBuffer(header_1, nread_1)) {
            log_err("Failed to read the object header from %s from disk at %ld", fn_1, cur_1);
        } 

        obj_info_1 = get_obj_info(header_1, cur_1, f_1);

        if(obj_info_1->nbytes < 0) {
            cur_1 -= obj_info_1->nbytes;
            continue;
        }
    
        class_name_str_1 = string(obj_info_1->class_name);
       
        if(debug_mode) {        
            set<string>::iterator it;
            cout << "Ignored classes are: ";
            for (it = ignored_classes.begin(); it != ignored_classes.end(); ++it) {
                cout << *it << " ";
            }
            cout << endl;
        }

        if (ignored_classes.find(class_name_str_1) == ignored_classes.end()) {
            objs_info.push_back(obj_info_1);
        }

        cur_1 += obj_info_1->nbytes;
    }

    // For each object in file 2, find if there exists an object which 
    // has same information in file 1. construct an table whose entry is
    // the pair of objects share same information from file 1 and file 2.
    // If there exists an object in file 2 which does not has matched
    // object in file 1, we say file 1 is not equal to file 2
    
    TFile *f_2 = new TFile(fn_2);

    Int_t nread_2 = nwheader;

    Long64_t cur_2 = HEADER_LEN, 
             f2_end = f_2->GetEND();

    char header_2[HEADER_LEN];

    Obj_info *obj_info_2;

    string class_name_str_2;
    vector<pair<Obj_info*, Obj_info*>> objs_pair;
    vector<pair<Obj_info*, Obj_info*>>::iterator vctr_p_itr;

    while(cur_2 < f2_end) {
        f_2->Seek(cur_2);
        
        if (cur_2 + nread_2 >= f2_end) {
            nread_2 = f2_end - cur_2 - 1;
        }
        
        if (f_2->ReadBuffer(header_2, nread_2)) {
            log_err("Failed to read the object header from %s from disk at %ld", fn_2, cur_2);
        }

        obj_info_2 = get_obj_info(header_2, cur_2, f_2);

        if(obj_info_2->nbytes < 0) {
            cur_2 -= obj_info_2->nbytes;
            continue;
        }
        
        class_name_str_2 = string(obj_info_2->class_name);
       
        if(debug_mode) {        
            set<string>::iterator it;
            cout << "Ignored classes are: ";
            for (it = ignored_classes.begin(); it != ignored_classes.end(); ++it) {
                cout << *it << " ";
            }
            cout << endl;
        }

        if (ignored_classes.find(class_name_str_2) == ignored_classes.end()) {
            // If current class is not in the ignored classes list
            vector<Obj_info*>::iterator vctr_itr;
            bool find_match = false;
            for (vctr_itr = objs_info.begin(); vctr_itr != objs_info.end(); ++vctr_itr) {
                if(roc->logic_cmp((*vctr_itr), obj_info_2)) {
                    pair<Obj_info*, Obj_info*> obj_pair((*vctr_itr), obj_info_2);
                    // every obj_info can only be used once
                    vctr_itr = objs_info.erase(vctr_itr);
                    objs_pair.push_back(obj_pair);
                    log_f << obj_info_2 << "is compared" << endl;
                    find_match = true;
                    break;
                } 
            } 

            if (!find_match) {
                // does not found matched object in file 1          
                log_f << " Cannot find matched object for the instance of " << 
                    obj_info_2->class_name << " in " << fn_2 << " at " << cur_2 << endl;
    
                logic_eq = false; 
                strict_eq = false;
                exact_eq = false;
                goto end;
            }
        }

        cur_2 += obj_info_2->nbytes;
    }
    // After iterating all objects in file 2, if there are obj_info left in file 1, file 1 
    // is not logically equal to file 2
    if(!objs_info.empty()) {
        vector<Obj_info*>::iterator vctr_itr; 
        for(vctr_itr = objs_info.begin(); vctr_itr != objs_info.end(); ++vctr_itr) {
            log_f << " Cannot find matched object for the instance of " << 
                (*vctr_itr)->class_name << " in " << fn_1 << " at " << (*vctr_itr)->seek_key << endl;
            delete (*vctr_itr);
        }
        logic_eq = false; 
        strict_eq = false;
        exact_eq = false;
        goto end;
    }

    // Compare the two objects in same entry. If the two objects are 
    // strictly/exactly equal to each other, we say the entry is 
    // strictly/exactly agreed. If every entry is strictly/exactly agreed, 
    // we say that file 1 is strictly/exactly equal to file 2. 

    for(vctr_p_itr = objs_pair.begin(); vctr_p_itr != objs_pair.end(); ++vctr_p_itr) {

        if(!roc->strict_cmp( (*vctr_p_itr).first, f_1, (*vctr_p_itr).second, f_2)) {

            log_f << " Instance of " << (*vctr_p_itr).first->class_name << 
                " in "<< fn_1 << " at " << (*vctr_p_itr).first->seek_key
                << " is NOT STRICTLY EQUAL to " << " Instance of " 
                << (*vctr_p_itr).second->class_name << " class in " << fn_2 << " at " 
                << (*vctr_p_itr).second->seek_key << endl;

            strict_eq = false;
            exact_eq = false;

        } else {

            if(!roc->exact_cmp((*vctr_p_itr).first, (*vctr_p_itr).second)) {

               log_f << " Instance of " << (*vctr_p_itr).first->class_name << 
                   " in "<< fn_1 << " at " << (*vctr_p_itr).first->seek_key
                   << " is NOT EXACTLY EQUAL to " << " Instance of " 
                   << (*vctr_p_itr).second->class_name << " class in " << fn_2 << " at " 
                   << (*vctr_p_itr).second->seek_key << endl;

                exact_eq = false;
            }

        }
    }

    if(!roc) {
        delete roc;
    }

end:
    for(vctr_p_itr = objs_pair.begin(); vctr_p_itr != objs_pair.end(); ++vctr_p_itr) {
        delete (*vctr_p_itr).first;
        delete (*vctr_p_itr).second;
    }

    tmr.reset();
    t = tmr.elapsed();
    log_f << endl; 
    log_f << "Comparison took: " << t << endl;

    log_f.close();

    if(exact_eq){
        return Exact_eq;
    } else if (strict_eq){
        return Strict_eq;
    } else if (logic_eq){
        return Logic_eq;
    } else {
        return Not_eq;
    }

error:
    exit(1);

}
