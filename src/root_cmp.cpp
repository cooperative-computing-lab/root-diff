#include <unistd.h>
#include <cstring>
#include "root_file_comparator.h"

void usage() {
    cout << endl;
    cout << "Use: root_cmp [options] -- command-line-and-options" << endl;
    cout << endl;
    cout << "-m         Specify compare mode (i.e. CC, UC, RC)." << endl;
    cout << "-f         Specify input files (i.e. -f file1,file2)." << endl;
}

int main(int argc, char *argv[]) {

    bool is_equal = true;
    bool no_opts = true;
    int opt = 0;
    string compare_mode = "CC";
    char *tmp_f_name = NULL, *fn1 = NULL, *fn2 = NULL;
    Rootfile_comparator rfc = Rootfile_comparator();

    while((opt = getopt(argc, argv, "hf:m:")) != -1) {

        switch(opt) {
            case 'm':
                no_opts = false;
                compare_mode = optarg;
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

    is_equal = rfc.root_file_cmp(fn1, fn2, compare_mode.c_str());
    
    if(is_equal) {
        printf("%s is EQUAL to %s\n", fn1, fn2);
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
