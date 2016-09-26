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
    left_events, left_messages, right_events, right_messages = edfread.fread(
        filename, ignore_samples, filter, split_char,
        properties_filter=list(properties_filter))

    if len(left_messages)>0:
        left_messages = pd.DataFrame(left_messages)
    else:
        left_messages = None
    if len(right_messages)>0:
        right_messages = pd.DataFrame(right_messages)
    else:
        right_messages = None

    if not ignore_samples:
        def join(events):
            # Join samples and events into one big data frame
            frames = []
            for event in left_events:
                samples = pd.DataFrame(event['samples'])
                samples['sample_time'] = samples['time']
                for key in set(event.keys()) - set(['samples']):
                    samples[key] = event[key]
                frames.append(samples)
            return pd.concat(frames)

    return join(left_events), left_messages, join(right_events), right_messages



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


def save_human_understandable(data, path):
    f = h5py.File(path, 'w')
    try:
        fm_group = f.create_group('edf')
        for field in data.columns:
            try:
                fm_group.create_dataset(field, data=data[field])
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

    events, messages = pread(args.edffile, ignore_samples=args.ignore_samples)
    events = trials2events(events, messages)
    if args.easy_hdf:
        save_human_understandable(events, args.outputfile)
    else:
        events.to_hdf(args.outputfile, 'edf', mode='w', complevel=9,
                      complib='zlib')
