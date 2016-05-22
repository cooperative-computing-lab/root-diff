#! /usr/bin/env python

import sys
import argparse
import numpy as np
import ROOT

def create(file, dir, label, shift, scale, index):

    x = np.zeros(1, dtype=float)
    hist_1 = ROOT.TH1F('hist', 'histogram of {}'.format(label), 100, -1.5, 1.5)
    tree_1 = ROOT.TTree('tree', 'tree of {}'.format(label))
    tree_1.Branch(label, x, '{}/D'.format(label))

    y = np.zeros(1, dtype=float)
    hist_2 = ROOT.TH1F('hist_1', 'histogram of {}'.format(label), 100, -1.5, 1.5)
    tree_2 = ROOT.TTree('tree_1', 'tree of {}'.format(label))
    tree_2.Branch(label, x, '{}/D'.format(label))

    if (scale == 'kb'):
        loop_num = 10000
    elif (scale == 'mb'):
        loop_num = 1000000
    elif (scale == 'gb'):
        loop_num = 70000000

    for i in xrange(loop_num):
        x[0] = shift
        hist_1.Fill(x)
        tree_1.Fill()

        y[0] = 2 + shift
        hist_2.Fill(y)
        tree_2.Fill()

    if index == 1:
        file.Write("hist_1")
        file.Write("hist_2")
    else:
        file.Write("hist_2")
        file.Write("hist_1")

def parse_arg(arg_lst):
    parser = argparse.ArgumentParser(description='Create benchmark ROOT files.')
    parser.add_argument('--scale', '-s', choices=['kb', 'mb', 'gb'], help='scale of output file')
    parser.add_argument('--prefix', '-p', help='prefix of output files')
    return parser.parse_args(arg_lst)

def gen_root_file(fn, index, scale):
    f = ROOT.TFile(fn, 'RECREATE')
    f.mkdir('example')
       
    v = ROOT.TLorentzVector(1.0, 2.0, 3.0, 4.0)
    v.Write('vector')
    
    create(f, '/', 'a', 0.5, scale, index)
    create(f, 'example', 'b', 1.0, scale, index)

def main(cmd_argvs):
    args = parse_arg(cmd_argvs[1:])
    fn_1 = "{}_1.root".format(args.prefix)
    fn_2 = "{}_2.root".format(args.prefix)

    if args.scale:
        scale = args.scale
    else:
        scale = 'kb'
    
    gen_root_file(fn_1, 1, scale)
    gen_root_file(fn_2, 2, scale)

if __name__ == "__main__":
    main(sys.argv)
