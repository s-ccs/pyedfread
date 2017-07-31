pyedfread
=========

A utility that parses SR research EDF data files into pandas DataFrames.



Requirements
============
 - python 3.6 recommended. Not tested with 2.7 anymore, but probably works.
 - edf access api. Download from SR-Research support forum,
   or install via apt-get on Ubuntu.
 - cython. Use conda/pip etc. to install
 - pandas + h5py. Required to run command line scripts. You can still parse edfs
   into numpy array and list of dicts without pandas.

 > I do not include the SR Research EDF Access API header files and libraries.
 > These are needed to compile pyedfread with cython. If you use a mac you can
 > download a package from the SR-Research support forum. If you use Ubuntu you
 > can install them via apt-get. The setup.py might not run properly on a linux
 > box, because I don't have one around for testing.
 >
 > If you use any other linux distribution, download the API files and place
 > libedfapi.so in lib/ and \*.h files in include. Setup.py should be able to
 > work with this.

Setup
=====
Run  'python setup.py install' to compile and install. This will install the
python library and a command line script to parse edfs.


Usage
=====

pyedfread can be used on the command line (read_edf) or called from
within python.

From python
-----------

After compilation run the following lines for a quick test.

    >>> from pyedfread import edf
    >>> samples, events, messages = edf.pread('SUB001.EDF')

This opens SUB001.EDF and parses it three two DataFrames:

 - samples contain individual samples.
 - events contains fixation and saccade definitions
 - messages contains meta data associated with each trial.

To add the trial meta data into the eye tracking data run:

    >>> events = edf.trials2events(events, messages)

pyedfread allows to select which meta data you want to read from your edf file.
This happens through the 'filter' argument of edf.pread / edfread.fread. It can
contain a list of 'message' identifiers. A message identifier in the EDF is
trial metadata injected into the data stream during eye tracking. If
for example after the start of a trial you send the message "condition 1", you
can add 'condition' to the filter list and edfread will automagically add a
field condition with value 1 for each trial to the messages structure. Of course,
if the value varies across trials this will be reflected in the messages
structure. This is what it looks like in python code:

	>>> samples, events, messages = edf.pread('SUB001.EDF', ignore_samples=True, filter=['condition'])

If filter='all', pyedfread saves all messages it can parse.

In earlier versions of pyedfread one could also specify which sample properties
to pull out from the data stream. This has been deprecated.

The column names map almost directly to C structure names in the EDF C API. To
understand column content check the edf acces api documentation (2.1.1 FSAMPLE
Structure + 2.1.2 FEVENT Struct.) and see the examples listed below. Column names
for each eye are suffixed by 'left' or 'right' respectively.

Some examples for samples:

- time: time stamp of sample
- flags: flags to indicate contents
- px: pupil x, y
- py
- hx: headref x, y
- hy
- pa: pupil size / area
- gx: screen gaze x, y
- gy
- rx: screen pixels per degree
- ry
- status: status flags
- input: extra input word
- buttons: button state & change
- htype: head-tracker data
- hdata: Not included
- errors: process error flags
- gxvel: gaze x,y velocity
- gyvel
- hxvel: headref x,y velocity
- hyvel
- rxvel: raw x, y velocity
- ryvel
- fgxvel: fast gaze x, y velocity
- fgyvel
- fhxvel: fast headref x, y velocity
- fhyvel
- frxvel: fast raw x, y velocity
- fryvel

Some examples for events:

 - time - time point where event occured
 - type - event type
 - eye - eye: 0=left, 1=right
 - start - start time
 - end - end time
 - gavx, gavy -  average gaze location


Command line
============

If you have an EDF file with "standard" meta data (e.g. key and value are seperated by a
blank) you can call

	$> read_edf SUB001.EDF sub001.hdf

The .hdf file is a valid hdf5 file that can be read into matlab. The default is
to simplify the HDF file by replacing strings with numbers. The original strings
are saved as attributes in the HDF file. Fields that contain python objects (e.g.
arrays, lists etc.) are skipped.

Run 'read_edf -h' for some help.


Matlab
======

To read a datamat into matlab do this:

    >> info = h5info('SUB001.hdf');
    >> info.Groups.Datasets
    >> field = h5read('SUB001.hdf', '/events/gavx');


License
=======

BSD License, see LICENSE file
