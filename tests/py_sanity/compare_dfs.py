import pandas as pd
import numpy as np
import sys;

dfs=list();
fns=list();
for a in sys.argv[1:]:
    fns.append(a);
    df=pd.read_csv(a);
    df = df[ [c for c in df.columns if c!='eye'] ];
    dfs.append( df);
    pass;
#df1 = pd.read_csv('test1')
#df2 = pd.read_csv('test2')

#for c in df1.columns:
#    np.isclose( df1[c], df2[c] ) == False


for i,df1 in enumerate(dfs):
    for j,df2 in enumerate(dfs):
        if( j >= i ):
            # skip lower triangle (redundant)
            continue;
        print("COMPARING: ", fns[i], fns[j]);
        
        if( list(df1.columns) != list(df2.columns) ):
            print(fns[i], fns[j]);
            raise Exception("COLUMNS not equal!");
        
        for c in df1.columns:
            closerows = np.isclose( df1[c], df2[c] ) | (np.isnan(df1[c]) & np.isnan(df2[c]) );
            if( np.any(
                    closerows == False
            )
               ):
                badrows = ~closerows;
                nbad = np.sum(badrows);
                print("[{} - {}]  --   Column {} fails ({}/{} bad)".format(fns[i], fns[j], c, nbad, len(df1.index)));
                
                bad1 = df1[ badrows ][c];
                bad2 = df2[ badrows ][c];
                #for x,y in zip(bad1, bad2):
                #    print("{} <-> {}".format(x,y));
                #    pass;
                pass;
            pass;
        pass;
    pass;
