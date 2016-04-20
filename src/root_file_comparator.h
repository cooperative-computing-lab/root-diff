#include "root_obj_comparator.h"
#include "timer.h"
#include "unistd.h"
#include <fstream>
#include <ctime>

typedef enum Agree_lv {
    Not_eq,
    Logic_eq,
    Strict_eq,
    Exact_eq 
} Agree_lv;

class Rootfile_comparator
{
public:
    Agree_lv root_file_cmp(char *f_1, char *f_2, 
            const char *mode, const char *log_fn);
};
