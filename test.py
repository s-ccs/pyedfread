import numpy as np
import edfread

if __name__ == '__main__':
    events, messages = edfread.fread('SUB001.EDF', ignore_samples=True)
    edfread.trials2events(events, messages)
    events.save('sub001.datamat')
