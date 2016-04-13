#include "root_file_comparator.h"

Agree_lv Rootfile_comparator::root_file_cmp(char *fn_1, char *fn_2, 
        const char *mode, const char *log_fn) 
{
    Rootobj_comparator *roc;   

    if (!strcmp(mode, "CC")) {
        roc = new Cmprs_comparator();
    } else if (!strcmp(mode, "UC")) {
        roc = new Uncmprs_comparator();
    } else if (!strcmp(mode, "RC")) {
        roc = new Reprod_comparator();
    } else {
        cout << "unknown option" <<  mode << endl; 
        exit(1);
    } 

    TFile f_1(fn_1), f_2(fn_2);

    TIter n_1(f_1.GetListOfKeys()),
          n_2(f_2.GetListOfKeys());

    TKey *k_1, *k_2;

    bool logic_eq = true, strict_eq = true, exact_eq = true;
   
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
