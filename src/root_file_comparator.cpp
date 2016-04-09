#include "root_file_comparator.h"

bool Rootfile_comparator::root_file_cmp(char *fn_1, char *fn_2, const char *mode) 
{
 
    Cmprs_comparator roc;   

    if (strcmp(mode, "CC") == 0) {
        Cmprs_comparator roc = Cmprs_comparator();
    } else if (strcmp(mode, "UC") == 0) {
        Uncmprs_comparator roc = Uncmprs_comparator();
    } else if (strcmp(mode, "RC") == 0) {
        Reprod_comparator roc = Reprod_comparator();
    } else {
        cout << "unknown option" <<  mode << endl; 
        exit(1);
    } 

    TFile f_1(fn_1), f_2(fn_2);

    TIter n_1(f_1.GetListOfKeys()),
          n_2(f_2.GetListOfKeys());

    TKey *k_1, *k_2;

    while(true) {
        k_1 = (TKey *)n_1();
        k_2 = (TKey *)n_2(); 

        if (!k_1 || !k_2){
            break;
        }

        if (!roc.logic_cmp(k_1, k_2)) {
            cout << fn_1 << " is NOT LOGICALLY EQUAL to " << fn_2 << endl;
            return false;
        }

        if (!roc.strict_cmp(k_1, k_2)) {
            cout << fn_1 << " is NOT STRICTLY EQUAL to " << fn_2 << endl;
            return false;
        }

        if (!roc.exact_cmp(k_1, k_2)) {
            cout << fn_1 << " is NOT EXACTLY EQUAL to " << fn_2 << endl;
            return false;
        }
    } 

    if ((!k_1 && k_2) || (k_1 && !k_2)) {
        cout << "Number of objects in " << fn_1 << " and " << fn_2 << " are not equal."<< endl;
        return false;
    }

    return true;
}
