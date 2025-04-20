#!/usr/bin/env python3

'''

Simple script which loads an EDF filename from command line
argument and saves samples, messages, events as named csvs.
Writes to same path as input .edf file with appended .samples.csv or
.events.csv or .messages.csv.

'''

def main():
    import sys
    import pandas as pd
    import pyedfread
    import numpy as np
    if( len(sys.argv) != 3 ):
        print("Usage: [THISSCRIPT] [EDFFILEPATH] [NROWS]");
        return 1;
    
    incsvpath = sys.argv[1];
    nrows = int(sys.argv[2]);
    sampdf, evtdf, msgdf = pyedfread.read_edf( incsvpath );
    
    sampdf = sampdf.iloc[ :nrows ];
    # Just to make saving smaller, only save 5000 samples
    #if( len(sampdf.index) > 10000 ):
        # For np r_: https://stackoverflow.com/questions/39393856/python-pandas-slice-dataframe-by-multiple-index-ranges
    #    sampdf = sampdf.iloc[np.r_[:5000, -5000:]];
    #    pass;
    
    sampdf.to_csv( incsvpath + '.samples.csv', index=False );
    evtdf.to_csv( incsvpath + '.events.csv', index=False );
    msgdf.to_csv( incsvpath + '.messages.csv', index=False );
        
    return 0;


if __name__=='__main__':
    exit( main() );
    pass;
