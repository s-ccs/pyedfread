import sys
from pyedfread import edfread
import numpy as np
import pandas as pd
import h5py


def pread(filename,
          ignore_samples=False,
          filter=[],
          split_char=' ',
          properties_filter=['type', 'time', 'sttime',
                             'entime', 'gx', 'gy', 'gstx', 'gsty', 'genx', 'geny',
                             'gxvel', 'gyvel', 'start', 'end', 'gavx', 'gavy', 'eye']):
    '''
    Parse an EDF file into a pandas.DataFrame.

    Input is the same as fread(...)
    '''
    if pd is None:
        raise RuntimeError('Can not import pandas.')
    properties_filter = set(properties_filter).union(set(['time', 'start']))
    samples, events, messages = edfread.fread(
        filename, ignore_samples,
        filter, split_char,
        properties_filter=list(properties_filter))
    events = pd.DataFrame(events)
    messages = pd.DataFrame(messages)
    samples = pd.DataFrame(np.asarray(samples), columns=edfread.sample_columns)
    return samples, events, messages


def join(events, ignore_samples):
    if len(events)==0 or events is None:
        return None
    if ignore_samples:
        return pd.DataFrame(events)
    # Join samples and events into one big data frame
    frames = []
    for event in events:
        samples = pd.DataFrame(event['samples'])
        samples['sample_time'] = samples['time']
        for key in set(event.keys()) - set(['samples']):
            samples[key] = event[key]
        frames.append(samples)
    return pd.concat(frames)

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


def join_left_right(left, right):
    if left is not None:
        left.rename(columns=dict((k, 'left_'+k) for k in left), inplace=True)
    if right is not None:
        right.rename(columns=dict((k, 'right_'+k) for k in right), inplace=True)
    if left is None:
        events = right
        events.loc[:, 'trial'] = events.loc[:, 'right_trial']
        del events['right_trial']
    elif right is None:
        events = left
        events.loc[:, 'trial'] = events.loc[:, 'left_trial']
        del events['left_trial']
    else:
        events = pd.concat((left.set_index('left_sample_time'),
                          right.set_index('right_sample_time'))).reset_index()
        events.loc[:, 'trial'] =   events.loc[:, 'right_trial']
        events.rename(columns={'index':'sample_time'}, inplace=True)
        del events['right_trial']
        del events['left_trial']
    return events


def get_list(subjects, pread=lambda x: pread(x)):
    dfs = []
    for snum, filename in subjects.iteritems():
        le, re, messages = pread(filename, filter='all')
        messsages = remove_time_fields(messages)
        events = join_left_right(le, re)
        df = trials2events(events, messages)
        df['SUBJECTINDEX'] = snum
        dfs.append(remove_time_fields(df))
    return dfs


def save_human_understandable(data, path, name):
    f = h5py.File(path, 'w')
    try:
        fm_group = f.create_group('edf')
        for field in data.columns:
            try:
                fm_group.create_dataset(field, data=data[field],
                    compression="gzip", compression_opts=1, shuffle=True)
            except TypeError:
                # Probably a string that can not be saved in hdf.
                # Map to numbers and save mapping in attrs.
                column = data[field]
                mapping = dict((key, i) for i, key in enumerate(np.unique(column)))
                fm_group.create_dataset(field, data=np.array([mapping[val] for val in column]))
                fm_group.attrs[field+'mapping'] = str(mapping)
    finally:
        f.close()


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("edffile", help='The EDF file you would like to parse')
    parser.add_argument("outputfile", help='Where to save the output')
    parser.add_argument("-e", "--easy_hdf", action='store_true', default=False,
                        help='Structure hdf output to be easily readable from e.g. matlab. Default just dumps a pandas DataFrame to hdf.')
    parser.add_argument("-i", "--ignore-samples", action='store_true', default=False,
                        help='Should the individual samples be stored in the output? Default is no, ie. ignore-samples=True')
    args = parser.parse_args()

    left, right, messages = pread(args.edffile, ignore_samples=args.ignore_samples)
    left = trials2events(left, messages)
    if args.easy_hdf:
        if left is not None:
            save_human_understandable(events, args.outputfile, 'left')
        if right is not None:
            save_human_understandable(right, args.outputfile, 'right')

    else:
        if left is not None:
            left.to_hdf(args.outputfile, 'left', mode='w', complevel=9,
                    complib='zlib')
        if right is not None:
            right.to_hdf(args.outputfile, 'right', mode='w', complevel=9,
                    complib='zlib')
