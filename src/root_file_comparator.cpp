#include "root_file_comparator.h"

Agree_lv Rootfile_comparator::root_file_cmp(char *fn_1, char *fn_2, const char *mode) 
{
    Rootobj_comparator *roc;   

    if (strcmp(mode, "CC") == 0) {
        roc = new Cmprs_comparator();
    } else if (strcmp(mode, "UC") == 0) {
        roc = new Uncmprs_comparator();
    } else if (strcmp(mode, "RC") == 0) {
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

    while(true) {
        k_1 = (TKey *)n_1();
        k_2 = (TKey *)n_2(); 

        if (!k_1 || !k_2){
            break;
        }

        if (!roc->logic_cmp(k_1, k_2)) {
            //cout << fn_1 << " is NOT LOGICALLY EQUAL to " << fn_2 << endl;
            logic_eq = false;
            strict_eq = false;
            exact_eq = false; 
        }

        if (!roc->strict_cmp(k_1, k_2)) {
            //cout << fn_1 << " is NOT STRICTLY EQUAL to " << fn_2 << endl;
            cout << k_1->GetClassName() << " is NOT EQUAL to " << k_2->GetClassName() << endl;
            strict_eq = false;    
            exact_eq = false;
        }

        if (!roc->exact_cmp(k_1, k_2)) {
            //cout << fn_1 << " is NOT EXACTLY EQUAL to " << fn_2 << endl;
            exact_eq = false;
        }
    } 

    if ((!k_1 && k_2) || (k_1 && !k_2)) {
        cout << "Number of objects in " << fn_1 << " and " << fn_2 << " are not equal."<< endl;
        delete roc; 
        return Not_eq;
    }

    delete roc;
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
