#include <unistd.h>
#include <string.h>
#include "root_comparator.h"

void usage() {
    cout << endl;
    cout << "Use: root_cmp [options] -- command-line-and-options" << endl;
    cout << endl;
    cout << "-l         Specify comparison level (i.e. logic, exact, strict)." << endl;
    cout << "-f         Specify input files (i.e. -f file1,file2)." << endl;
}

int main(int argc, char *argv[]) {

    bool is_equal = true;
    bool no_opts = true;
    int opt = 0;
    string class_type = "exact";
    char *tmp_f_name = NULL, *fn1 = NULL, *fn2 = NULL;
    while((opt = getopt(argc, argv, "hf:l:")) != -1) {
        switch(opt) {
            case 'l':
                no_opts = false;
                class_type.assign(optarg);
                break;
            case 'h':
                no_opts = false;
                usage();
                exit(0);
            case 'f':
                no_opts = false;
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
                no_opts = false;
                usage();
                goto error;
        }
        
    }

    if(no_opts) {
        usage();
        goto error;
    }

    if (!class_type.compare("exact")) {
        Exact_comparator ec;
        is_equal = ec.root_file_cmp(fn1, fn2);
    } else if (!class_type.compare("logic")) {
        Logic_comparator lc;
        is_equal = lc.root_file_cmp(fn1, fn2);
    } else if (!class_type.compare("strict")) {
        Strict_comparator sc;
        is_equal = sc.root_file_cmp(fn1, fn2);
    } else {
        if(fn1 != NULL) {
            delete fn1;
        } 

        if(fn2 != NULL) {
            delete fn2;
        }
        usage();
        goto error;
    }

    if(is_equal) {
        printf("%s is EQUAL to %s\n", fn1, fn2);
    } else {
        printf("%s is NOT EQUAL to %s\n", fn1, fn2);
    }

    if(fn1 != NULL) {
        delete fn1;
    } 

    if(fn2 != NULL) {
        delete fn2;
    }

    exit(0);

error:
    exit(1);
}
