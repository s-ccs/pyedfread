from pyedfread import edf_read
import numpy as np
import pandas as pd
import h5py
import os
import argparse


def read_edf(
    filename,
    ignore_samples=False,
    message_filter=None,
    trial_marker="TRIALID",
    ftime=False
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

    message_filter : list of str, optional
        Messages are kept only if they start with one of these strings.

    trial_marker : str, optional
        Messages that start with this string will be assumed to
        indicate the start of a trial.

    ftime: bool, optional
        If true, times are converted to floating point (from default int32) and 0.5 msec intervals are added
        for approriate 2000 hz recordings. Otherwise, int times are simply doubled up for 2k hz recordings.

    Returns
    -------
    samples : pandas.DataFrame
        Sample information, including time, x, y, and pupil size.

    events : pandas.DataFrame
        Event information, including saccades and fixations.

    messages : pandas.DataFrame
        Message information.
    """
    if not os.path.isfile(filename):
        raise RuntimeError(f"File does not exist: {filename}")

    samples, events, messages = edf_read.parse_edf(
        filename, ignore_samples, message_filter, trial_marker
    )
    events = pd.DataFrame(events)
    messages = pd.DataFrame(messages)
    samples = pd.DataFrame(samples)
    
    if( True == ftime ):
        samples = edf_read.samples_to_ftimes(samples);
        pass;
    
    ## Reorder samples column to be in same order as previously (based on FSAMPLE struct)
    samples = samples[ list(edf_read.sample_columns) ];
    
    return samples, events, messages


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
