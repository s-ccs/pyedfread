from . import edfread
import numpy as np
import pandas as pd
import h5py
import os


def pread(filename,
          ignore_samples=False,
          filter='all',
          split_char=' ',
          trial_marker=b'TRIALID',
          meta={}):
    '''
    Parse an EDF file into a pandas.DataFrame.

    EDF files contain three types of data: samples, events and messages. Samples
    are what is recorded by the eye-tracker at each point in time. This contains
    for example instantaneous gaze position and pupil size. Events abstract from
    samples by defining fixations, saccades and blinks. Messages contain meta
    information about the recording such as user defined information and
    calibration information etc.

    pread returns one pandas DataFrame for each type of information.

    Arguments:
    ---------

    ignore_samples : If true individual samples will not be saved, but only
        event averages.

    filter : List of strings.
        The SR system allows to send trial meta data messages into the data
        stream. This function decides which messages to keep by checking if the
        message string is in this filter list. Messages are split by
        'split_char' and the first part of the message is checked against the
        filter list. Example:
            Message is "beep_150" and split_char = '_' -> (beep, 150)
            Message is "beep 150" and split_char = ' ' -> (beep, 150)

    split_char : Character used to split metadata messages.
    
    meta : A dictionary to insert additional metadata to dataframes based on 
        key-value pairs.
    '''
    if not os.path.isfile(filename):
        raise RuntimeError('File "%s" does not exist' % filename)

    if pd is None:
        raise RuntimeError('Can not import pandas.')

    samples, events, messages = edfread.fread(
        filename, ignore_samples,
        filter, split_char, trial_marker)
    events = pd.DataFrame(events)
    messages = pd.DataFrame(messages)
    samples = pd.DataFrame(np.asarray(samples), columns=edfread.sample_columns)
    
    for key, value in meta.items():
        events.insert(0,key,value)
        messages.insert(0,key,value)
        samples.insert(0,key,value)
        
    return samples, events, messages


def remove_time_fields(events):
    '''
    Convenience function that drops the message send time fields.
    '''
    return events.drop([key for key in events.columns
                        if 'message_send_time' in key], axis=1)


def trials2events(events, messages):
    '''
    Matches trial meta information to individual events (e.g. fixations/saccades).
    '''
    return events.merge(messages, how='left', on=['trial'])


def save_human_understandable(samples, events, messages, path):
    '''
    Save HDF with explicit mapping of string to numbers.
    '''
    f = h5py.File(path, 'w')
    try:
        for name, data in zip(['samples', 'events', 'messages'],
                              [samples, events, messages]):
            fm_group = f.create_group(name)
            for field in data.columns:
                try:
                    fm_group.create_dataset(field,
                                            data=data[field],
                                            compression="gzip",
                                            compression_opts=1,
                                            shuffle=True)
                except TypeError:
                    # Probably a string that can not be saved in hdf.
                    # Map to numbers and save mapping in attrs.
                    column = data[field].values.astype(str)
                    mapping = dict((key, i)
                                   for i, key in enumerate(np.unique(column)))
                    fm_group.create_dataset(field,
                                            data=np.array([mapping[val]
                                                           for val in column]))
                    fm_group.attrs[field + 'mapping'] = str(mapping)

    finally:
        f.close()   
