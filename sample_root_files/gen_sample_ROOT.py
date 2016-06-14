#! /usr/bin/env python

import sys
import argparse
import numpy as np
import ROOT

def create(file, dir, label, shift, scale, mode):
    file.cd(dir)

    x = np.zeros(1, dtype=float)
    hist = ROOT.TH1F('hist_{}'.format(label), 'histogram of {}'.format(label), 100, -1.5, 1.5)
    tree = ROOT.TTree('tree_{}'.format(label), 'tree of {}'.format(label))
    tree.Branch(label, x, '{}/D'.format(label))

    if (scale == 'kb'):
        loop_num = 10000
    elif (scale == 'mb'):
        loop_num = 1000000
    elif (scale == 'gb'):
        loop_num = 70000000

    for i in xrange(loop_num):
        if mode == 'random':
            x[0] = np.random.randn() + shift
            tree.Fill()
            hist.Fill(x)
        else:
            x[0] = shift
            hist.Fill(x)
            tree.Fill()

    file.Write()

def parse_arg(arg_lst):
    parser = argparse.ArgumentParser(description='Create benchmark ROOT files.')
    parser.add_argument('--mode', '-m', choices=['random', 'fixed'], help='make output random or fixed')
    parser.add_argument('--scale', '-s', choices=['kb', 'mb', 'gb'], help='scale of output file')
    parser.add_argument('--shift', '-i', help='shift value')
    parser.add_argument('--file', '-f', help='name of output file')
    return parser.parse_args(arg_lst)

def main(cmd_argvs):
    args = parse_arg(cmd_argvs[1:])
    f = ROOT.TFile(args.file, 'RECREATE')
    f.mkdir('example')
    
    v = ROOT.TLorentzVector(1.0, 2.0, 3.0, 4.0)
    v.Write('vector')
    
    create(f, '/', 'a', args.shift, args.scale, args.mode)
    create(f, 'example', 'b', args.shift, args.scale, args.mode)

if __name__ == "__main__":
    main(sys.argv)
