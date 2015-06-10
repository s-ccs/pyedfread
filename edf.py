import sys
import edfread

def usage():
    print 'Usage: python edf.py input_file output_file'
	

if __name__ == '__main__':
    if (len(sys.argv) == 2) and (sys.argv[1] == 'test'):
        events, messages = edfread.fread('SUB001.EDF', ignore_samples=True)
        edfread.trials2events(events, messages)
    	print events   
    
    elif len(sys.argv) == 3:
    	events, messages = edfread.fread(sys.argv[1], ignore_samples=False)
    	edfread.trials2events(events, messages)
    	events.save(sys.argv[2])
    
    else:
    	usage()
