from pyedfread import edf_read
import numpy as np
import pandas as pd
import h5py
import os
import argparse


def read_edf(
    filename,
    ignore_samples=False,
    filter="all",
    split_char=" ",
    trial_marker=b"TRIALID",
    meta=None,
):
    """
    Parse an EDF file into a pandas.DataFrame.

    EDF files contain three types of data: samples, events and
    messages. Samples are what is recorded by the eye-tracker at each
    point in time. This contains for example instantaneous gaze
    position and pupil size. Events abstract from samples by defining
    fixations, saccades and blinks. Messages contain meta information
    about the recording such as user defined information and
    calibration information etc.

    Parameters
    ----------
    filename : str
        Path to EDF file.

    ignore_samples : bool
        If true individual samples will not be saved, but only event
        averages.

    filter : list of str
        The SR system allows to send trial meta data messages into
        the data stream. This function decides which messages to keep
        by checking if the message string is in this filter list.
        Messages are split by 'split_char' and the first part of the
        message is checked against the filter list. Example:
            Message is "beep_150" and split_char = '_' -> (beep, 150)
            Message is "beep 150" and split_char = ' ' -> (beep, 150)

    split_char : str
        Character used to split metadata messages.

    trial_marker : bytearray
        Byte string at the start of messages to include.

    meta : dict
        A dictionary to insert additional metadata to dataframes based
        on key-value pairs.

    Returns
    -------
    samples : pandas.DataFrame
        Sample information, including time, x, y, and pupil size.

    events : pandas.DataFrame
        Event information, including saccades and fixations.

    messages : pandas.DataFrame
        Message information.
    """
    if meta is None:
        meta = {}
    if not os.path.isfile(filename):
        raise RuntimeError('File "%s" does not exist' % filename)

    samples, events, messages = edf_read.fread(
        filename, ignore_samples, filter, split_char, trial_marker
    )
    events = pd.DataFrame(events)
    messages = pd.DataFrame(messages)
    samples = pd.DataFrame(np.asarray(samples), columns=edf_read.sample_columns)

    for key, value in meta.items():
        events.insert(0, key, value)
        messages.insert(0, key, value)
        samples.insert(0, key, value)
    return samples, events, messages


def remove_time_fields(events):
    """Drop the message send time fields."""
    trimmed = events.drop(
        [key for key in events.columns if "message_send_time" in key], axis=1
    )
    return trimmed


def trials2events(events, messages):
    """Match trial meta information to individual events."""
    return events.merge(messages, how="left", on=["trial"])


def save_human_understandable(samples, events, messages, path):
    """Save HDF with explicit mapping of string to numbers."""
    f = h5py.File(path, "w")
    try:
        for name, data in zip(
            ["samples", "events", "messages"], [samples, events, messages]
        ):
            fm_group = f.create_group(name)
            for field in data.columns:
                try:
                    fm_group.create_dataset(
                        field,
                        data=data[field],
                        compression="gzip",
                        compression_opts=1,
                        shuffle=True,
                    )
                except TypeError:
                    # Probably a string that can not be saved in hdf.
                    # Map to numbers and save mapping in attrs.
                    column = data[field].values.astype(str)
                    mapping = dict((key, i) for i, key in enumerate(np.unique(column)))
                    fm_group.create_dataset(
                        field, data=np.array([mapping[val] for val in column])
                    )
                    fm_group.attrs[field + "mapping"] = str(mapping)
    finally:
        f.close()


def convert_edf():
    """Read an EDF file and write to an HDF file."""
    parser = argparse.ArgumentParser()
    parser.add_argument("edffile", help="The EDF file you would like to parse")
    parser.add_argument("outputfile", help="Where to save the output")
    parser.add_argument(
        "-p",
        "--pandas_hdf",
        action="store_true",
        default=False,
        help="Use pandas to store HDF. Default simplifies HDF strucutre significantly (e.g. map strings to numbers and skip objects)",
    )
    parser.add_argument(
        "-i",
        "--ignore-samples",
        action="store_true",
        default=False,
        help="Should the individual samples be stored in the output? Default is to read samples, ie. ignore-samples=False",
    )
    parser.add_argument(
        "-j",
        "--join",
        action="store_true",
        default=False,
        help="If True events and messages will be joined into one big structure based on the trial number.",
    )
    args = parser.parse_args()
    samples, events, messages = read_edf(args.edffile, ignore_samples=args.ignore_samples)

    if args.join:
        events = trials2events(events, messages)

    if not args.pandas_hdf:
        columns = [
            col
            for col in messages.columns
            if not (messages[col].dtype == np.dtype("O"))
        ]
        messages = messages.loc[:, columns]
        save_human_understandable(samples, events, messages, args.outputfile)
    else:
        events.to_hdf(args.outputfile, "events", mode="w", complevel=9, complib="zlib")
        samples.to_hdf(
            args.outputfile, "samples", mode="w", complevel=9, complib="zlib"
        )
        messages.to_hdf(
            args.outputfile, "messages", mode="w", complevel=9, complib="zlib"
        )
