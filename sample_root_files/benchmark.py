#! /usr/bin/env python

import argparse
import numpy as np
import ROOT

parser = argparse.ArgumentParser(description='Create benchmark ROOT files.')
parser.add_argument('mode', choices=['random', 'fixed'], help='make output random or fixed')
parser.add_argument('filename', help='name of output file')
args = parser.parse_args()

def create(file, dir, label, shift):
    file.cd(dir)

    x = np.zeros(1, dtype=float)
    hist = ROOT.TH1F('hist', 'histogram of {}'.format(label), 100, -1.5, 1.5)
    tree = ROOT.TTree('tree', 'tree of {}'.format(label))
    tree.Branch(label, x, '{}/D'.format(label))

    for i in xrange(1000):
        if args.mode == 'random':
            x[0] = np.random.randn() + shift
            tree.Fill()
            hist.Fill(x)
        else:
            x[0] = shift
            hist.Fill(x)
            tree.Fill()

    file.Write()

f = ROOT.TFile(args.filename, 'RECREATE')
f.mkdir('example')

v = ROOT.TLorentzVector(1.0, 2.0, 3.0, 4.0)
v.Write('vector')

create(f, '/', 'a', 0.5)
create(f, 'example', 'b', 1.0)
