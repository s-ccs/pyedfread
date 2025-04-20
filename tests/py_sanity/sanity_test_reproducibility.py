import pandas as pd
import numpy as np
import sys;
import time;
import pyedfread;

## Why would values change between "runs" (i.e. different process) and "repeats" in same process?
## Probably due to memory mapping etc., of the loaded EDFAPI library, when it is loaded in (dynamically?).
dfs=list();
NTRIES=3;
for n in range(NTRIES):
    s, e, m = pyedfread.read_edf( sys.argv[1] ) ;
    dfs.append(s);
    import pyedfread
    #time.sleep(30);
    pass;

tag = str(sys.argv[2]);

csvfns=list();
for i,df1 in enumerate(dfs):
    fn = '{}_df{}.csv'.format(tag,i);
    csvfns.append(fn);
    df1.to_csv(fn, index=False);
    for j,df2 in enumerate(dfs):
        if( j >= i ):
            # skip lower triangle (redundant)
            continue;
        
        if( list(df1.columns) != list(df2.columns) ):
            #print(fns[i], fns[j]);
            raise Exception("COLUMNS not equal!");
        
        for c in df1.columns:
            closerows = np.isclose( df1[c], df2[c] ) | (np.isnan(df1[c]) & np.isnan(df2[c]) );
            if( np.any(
                    closerows == False
            )
               ):
                print("[{} - {}]  --   Column {} fails".format(i, j, c));
                pass;
            pass;
        pass;
    pass;

print("SUCCEEDED in memory, now CSV");

dfs = list();
for fn in csvfns:
    s = pd.read_csv(fn);
    dfs.append(s);
    pass;

for i,df1 in enumerate(dfs):
    for j,df2 in enumerate(dfs):
        if( j >= i ):
            # skip lower triangle (redundant)
            continue;
        
        if( list(df1.columns) != list(df2.columns) ):
            print("FAIL COLUMN", fns[i], fns[j]);
            raise Exception("COLUMNS not equal!");
        
        for c in df1.columns:
            closerows = np.isclose( df1[c], df2[c] ) | (np.isnan(df1[c]) & np.isnan(df2[c]) );
            if( np.any(
                    closerows == False
            )
               ):
                print("[{} - {}]  --   Column {} fails".format(fns[i], fns[j], c));
                pass;
            pass;
        pass;
    pass;
