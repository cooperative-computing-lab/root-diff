#include <unistd.h>
#include <cstring>
#include "dbg.h"
#include "root_file_comparator.h"

void usage() {
    cout << endl;
    cout << "Use: root_cmp [options] -- command-line-and-options" << endl;
    cout << endl;
    cout << "-l         Write details to log (i.e. -l /path/to/logfile)" << endl;
    cout << "-m         Specify compare mode (i.e. CC, UC)." << endl;
    cout << "-f         Specify input files (i.e. -f file1,file2)." << endl;
    cout << endl;
}

int main(int argc, char *argv[]) {

    Agree_lv al = Not_eq;
    bool fn_opts = false;
    int opt = 0;
    string compare_mode = "CC";
    string cmp_mode_str = "COMPRESS COMPARE";
    string agree_lv = "LOGICAL";
    string log_fn = string();
    char *tmp_f_name = NULL, *fn1 = NULL, *fn2 = NULL;
    Rootfile_comparator rfc = Rootfile_comparator();

    while((opt = getopt(argc, argv, "hf:m:l:")) != -1) {

        switch(opt) {
            case 'l':
                log_fn = optarg;
                break;

            case 'm':
                compare_mode = optarg;
                break;

            case 'h':
                usage();
                exit(0);

            case 'f':
                fn_opts = true;
                tmp_f_name = strtok(optarg, ",");
                if (!tmp_f_name) {
                    cout << "Please specify two root files." << endl;
                    goto error;
                }
                fn1 = strdup(tmp_f_name); 
                tmp_f_name = strtok(NULL, ",");
                if (!tmp_f_name) {
                    cout << "Please specify two root files." << endl;
                    goto error;
                }
                fn2 = strdup(tmp_f_name);
                break;

            default:
                usage();
                goto error;

        }
    }

    if(fn_opts == false) {
        cout << "Please specified two root files." <<endl;
        usage();
        goto error;
    }

    if (log_fn.empty()) { 
        log_fn = "root_diff.log";
    }

    al = rfc.root_file_cmp(fn1, fn2, compare_mode.c_str(), log_fn.c_str());

    switch(al) {
        case Logic_eq:
            break;
        case Strict_eq:
            agree_lv = "STRICT"; 
            break;
        case Exact_eq:
            agree_lv = "EXACT";
            break;
        case Not_eq:
            break;
        default:
            cout << "Unknown Agreement Level" << endl;
            goto error;
    }

    if (!compare_mode.compare("UC")) {
        cmp_mode_str = "UNCOMPRESSED COMPARE";
    } else if (!compare_mode.compare("RC")) {
        cmp_mode_str = "REPRODUCE COMPARE";
    }

    // Output the comparison result
    cout << "-----------------------------------------------------------" << endl;
    cout << "file 1: " << fn1 << endl;
    cout << "file 2: " << fn2 << endl;
    cout << "The comparison mode is: " << cmp_mode_str << endl;
    
    if(al == Not_eq) {
        cout << "file 1 is NOT EQUAL to file 2." << endl;
    } else {
        cout << "file 1 is EQUAL to file 2." << endl;
        cout << "The agreement level is " << agree_lv << endl;
    }
    // if log file is specified
    
    cout << "Details can be found in " << log_fn << endl;
    cout << "-----------------------------------------------------------" << endl;

    if(fn1 != NULL) {
        delete [] fn1;
    } 

    if(fn2 != NULL) {
        delete [] fn2;
    }

    exit(0);

error:
    if(fn1 != NULL) {
        delete [] fn1;
    } 

    if(fn2 != NULL) {
        delete [] fn2;
    }
    exit(1);
}

