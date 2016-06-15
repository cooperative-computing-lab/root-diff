#include <unistd.h>
#include <string>
#include "dbg.h"
#include "root_file_comparator.h"

using namespace std;

bool debug_mode;

static void get_ignored_classes(set<string> &ignored_classes, char *ignored_classes_fn) 
{
    ifstream classes_fn(ignored_classes_fn);        
    string curr_line;
    while(getline(classes_fn, curr_line)) {
        ignored_classes.insert(curr_line);    
    }
}

static inline void usage() 
{
    cout << endl;
    cout << "Use: root_cmp [options] -- command-line-and-options" << endl;
    cout << endl;
    cout << "-c         Path to the user specified file, which records ignored classes" << endl; 
    cout << "-l         Enable log mode and write details to log (i.e. -l /path/to/logfile)" << endl;
    cout << "-m         Specify compare mode (i.e. CC, UC)." << endl;
    cout << "-d         Enable debug mode." << endl;
    cout << endl;
}

int main(int argc, char *argv[]) 
{
    Agree_lv al = Not_eq;
    debug_mode = false;
    int opt = 0;
    string compare_mode = "CC";
    string cmp_mode_str = "COMPRESS COMPARE"; 
    string agree_lv = "LOGICAL"; 
    string log_fn = string("root_diff.log");
    char *fn1 = NULL, *fn2 = NULL;
    char *ignored_classes_fn = NULL;

    // Insert three types of class that will be ignored
    set<string> ignored_classes; 
    ignored_classes.insert("TFile");
    ignored_classes.insert("TDirectory");
    ignored_classes.insert("KeysList");
    ignored_classes.insert("StreamerInfo");
    ignored_classes.insert("FreeSegments");

    Rootfile_comparator rfc = Rootfile_comparator();

    extern char *optarg;
    extern int optind, opopt;
    int num_root_files = 0;
    int rc = 0;

    while((opt = getopt(argc, argv, "hf:m:l:c:d")) != -1) {

        switch(opt) {

            case 'l':
                log_fn = optarg;
                break;

            case 'm':
                compare_mode = optarg;
                break;

            case 'h':
                usage();
                return 0; 

            case 'c':
                ignored_classes_fn = optarg; 
                get_ignored_classes(ignored_classes, ignored_classes_fn);     
                break;

            case 'd':
                debug_mode = true;
                break;

            default:
                usage();
                goto error;

        }
    }

    for (; optind<argc; optind++) {
        rc = access(argv[optind], R_OK);
        if ( rc == 0 && num_root_files == 0) {
            fn1 = strdup(argv[optind]);
        }
        if ( rc == 0 && num_root_files == 1) {
            fn2 = strdup(argv[optind]);
        }

        num_root_files ++;

        if ( rc != 0) {
            log_err("%s is not accessible.", argv[optind]);
            goto error; 
        }
    }

    // Check if there are two root files specified
    if (num_root_files != 2) {
        log_err("Please specify two root files.");
        goto error;
    }

    // Compare two root files
    al = rfc.root_file_cmp(fn1, fn2, compare_mode.c_str(), log_fn.c_str(), ignored_classes);

    // Check the agreement level
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
            log_err("Unknown Agreement Level"); 
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

    return 0; 

error:
    if(fn1 != NULL) {
        delete [] fn1;
    } 

    if(fn2 != NULL) {
        delete [] fn2;
    }
    return 1;
}
