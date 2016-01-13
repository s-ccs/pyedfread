pyedfread
=========

A utility that parses SR research EDF data files into pandas DataFrames.

pyedfread _needs to be compiled_ before it can be used.



Requirements
============

 - edf access api. Download from SR-Research support forum,
   or install via apt-get on Ubuntu.
 - cython. Use conda/pip etc. to install
 - pandas + h5py: Strongly recommended. You can still use pyedfread to
   parse your EDF file into a list of dicts (one for each trial).
 - progressbar-ipython / progressbar [optional]

 > I do not include the SR Research EDF Access API header files and libraries.
 > These are needed to compile pyedfread with cython. If you use a mac you can
 > download a package from the SR-Research support forum. If you use Ubuntu you
 > can install them via apt-get (but I've never done this), _but_ the setup.py
 > is not yet able to deal with this since I don't have a linux box at hand to
 > test how it works. Might need some tuning from your side.
 >
 > If you use any other linux distribution, download the API files and place
 > libedfapi.so in lib/ and \*.h files in include. Setup.py should be able to
 > work with this.

Setup
=====
Run  'python setup.py build_ext --inplace' to compile the package. Add the
pyedfread path to yout PYTHONPATH. Eventually this might be turned into a
package that can be installed via pip or so.


Usage
=====

pyedfread can be used on the command line (python edf.py) or called from
within python.

From python
-----------

After compilation run the following lines for a quick test.

    >>> import edf
    >>> events, messages = edf.pread('SUB001.EDF', ignore_samples=True)

This opens SUB001.EDF and parses it into two DataFrames:

 - events contains the eye tracking data for each fixation / saccade
 - messages contains meta data associated with each trial.

To add the trial meta data into the events structure run:

    >>> data = edf.trials2events(events, messages)

pyedfread allows to select which data you want to read from your edf file. This
happens with two arguments to edfread.fread / edf.pread. The 'filter' argument
can contain a list of 'message' identifiers. A message identifier in the EDF is
for example trial metadata injected into the data stream during eye tracking. If
for example after the start of a trial you send the message "condition 1", you
can add 'condition' to the filter list and edfread will automagically add a
field condition with value 1 for each trial to the messages structure. Of course,
if the value varies across trials this will be reflected in the messages
structure. This is what it looks like in python code:

	>>> events, messages = edf.pread('SUB001.EDF', ignore_samples=True, filter=['condition'])

If filter='all', pyedfread saves all messages it can parse.

You can also specify which properties of the eye tracking data you pull out. If
you want, for example, only the pupil and the gaze x position you can set the
'properties_filter' of the edfread.fread / edf.pread method:

	>>> events, messages = edf.pread('SUB001.EDF',
					 ignore_samples=True,
	        			 properties_filter=['gx', 'pa'])

To see how properties are named check the data2dict function in edfread.pyx and
the edf acces api documentation (2.1.1 FSAMPLE Structure + 2.1.2 FEVENT Struct.).
Some examples:

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

	$> python edf.py SUB001.EDF sub001.hdf

The .hdf file is a valid hdf5 file that can be read into matlab.


Matlab
======

To read a datamat into matlab do this:

    >> info = h5info('SUB001.hdf');
    >> info.Groups.Datasets
    >> field = h5read('SUB001.dm', '/edf_open_file/fieldname');


License
=======

BSD License, see LICENSE file
