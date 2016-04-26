#ifndef __ROOT_F_CMPTR__
#define __ROOT_F_CMPTR__

#include "root_obj_comparator.h"
#include "TDatime.h"
#include "RtypesCore.h"
#include "Bytes.h"
#include "dbg.h"
#include "timer.h"
#include "unistd.h"
#include <cstring>
#include <fstream>
#include <ctime>

#define NAME_LEN 512
#define HEADER_LEN 100

/*
 * Four agreement levels:
 * 1. NOT EQUAL
 * 2. LOGICALLY EQUAL
 * 3. STRICTLY EQUAL
 * 4. EXACTLLY EQUAL 
 */

typedef enum Agree_lv {
    Not_eq,
    Logic_eq,
    Strict_eq,
    Exact_eq 
} Agree_lv;


/*
 * The root file comparator class
 */
class Rootfile_comparator
{
public:

    /*
     * Get object information from the header (i.e. TKey)
     */
    Obj_info *get_obj_info(const char *header, long cur, const TFile *f);

    /*
     * Compare two root files and return the agreement level of the 
     * comparsion
     */
    Agree_lv root_file_cmp(char *f_1, char *f_2, 
            const char *mode, const char *log_fn);
};

#endif
