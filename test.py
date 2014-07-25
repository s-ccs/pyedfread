import numpy as np

def trials2events(events, messages, remove_time_fields = True):
    for key in messages.fieldnames():
        if key == 'trial':
            continue
        events.add_field(key, 
                np.empty(events.field(events.fieldnames()[0]).shape,
                dtype=messages.field(key).dtype))
    for trial in np.unique(messages.field('trial')):
        idt = events.trial ==  trial
        for key in messages.fieldnames():
            if key == 'trial':
                continue
            events.field(key)[idt] = messages.field(key)[trial]
    



if __name__ == '__main__':
    import edfread
    events, messages = edfread.fread('SUB001.EDF', ignore_samples=True)
    edfread.trials2events(events, messages)
    print events
