#!/bin/sh

tth_dir=/data/czheng2/sample_root/root_tth_ttjets/tth
ttjets_dir=/data/czheng2/sample_root/root_tth_ttjets/ttjets

tth_dat=tth_UC.dat
ttjets_dat=ttjets_UC.dat

gen_data_fn()
{
    for f in $1/*.root
    do
        fn_size=$(ls -lh $f | awk -F ' ' '{print $5}')
        size_num=$(echo $fn_size | sed 's/.$//')
        fn_basename=$(basename $f)
        # Call root_diff
        cmd="/usr/bin/time -p /afs/crc.nd.edu/user/c/czheng2/Work/ccl_work/root_diff/bin/root_diff -m CC $f $f 2>&1 >/dev/null | tail -n3"
        run_time=$(/usr/bin/time -p /afs/crc.nd.edu/user/c/czheng2/Work/ccl_work/root_diff/bin/root_diff -m UC $f $f 2>&1 >/dev/null | tail -n3)
        # Get the value of "real, user, sys"
        run_time_data=$(echo $run_time | awk -F ' ' '{print $2" "$4" "$6}')
        data_entry="$size_num $fn_basename $run_time_data"
        echo $data_entry >> $2
    done
}

main()
{
    gen_data_fn $tth_dir $tth_dat
    gen_data_fn $ttjets_dir $ttjets_dat
}

main
