##CMS root toolkits

This program can only on built on crc machines.
Before build please remove the gcc module and load the 
root, python/2.7.8 module by typing following commands:

    module rm gcc
    module load root python/2.7.8

Then you can build the program by executing

    make

After build, `root_cmp` in src/ can be used to compare the 
two root files, more details can be found through `bin/root_cmp -h`

Example:

`bin/root_cmp -f sample_root_files/f1 sample_root_files/f2`

`bin/root_cmp -l logic -f sample_root_files/f1 sample_root_files/f2`


