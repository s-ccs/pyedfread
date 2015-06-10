pyedfread
=========

A utility that parses a SR research EDF data file into a HDF5 file via 
ocupy.datmat as intermediary.

pyedfread needs to be compiled before it can be used.

Requirements
============

 - ocupy. Get it here: http://github.com/nwilming/ocupy
 - h5py. Python hdf 5 package. Use conda/pip/easy_install
 - edf access api. Download from SR-Research support forum,
   or install via apt-get on Ubuntu.
 - cython. Use conda/pip etc. to install


Setup
=====
Run  'python setup.py build_ext --inplace' to compile the package.
Add the pyedfread path to yout PYTHONPATH. 


Usage
=====

pyedfread can be used on the command line (python edf.py) or called from
within python.

From python
-----------

After compilation run the following lines for a quick test. 

    >>> import edfread
    >>> events, messages = edfread.fread('SUB001.EDF', ignore_samples=True)

This opens SUB001.EDF and parses it into two datamats:
 
 - events contains the eye tracking data for each fixation / saccade
 - messages contains meta data associated with each trial.

To add the trial meta data into the events structure run:

    >>> edfread.trials2events(events, messages)

pyedfread allows to select which data you want to read from your edf file. This
happens with two arguments to edfread.fread. The 'filter' argument can contain a 
list of 'message' identifiers. A message identifier in the EDF is for example 
trial metadata injected into the data stream during eye tracking. If for example 
after the start of a trial you send the message "condition 1", you can add 'condition'
to the filter list and edfread will automagically add a field condition with value 1 
for each trial to the messages structure. Of course, if the value varies across trials
this will be reflected in the messages structure. This is what it looks like in
python code:

	>>> events, messages = edfread.fread('SUB001.EDF', ignore_samples=True, filter=['condition'])

You can also specify which properties of the eye tracking data you can pull out. If
you want, for example, only the pupil and the gaze x position you can set the 
'properties_filter' of the edfread.fread method:

	>>> events, messages = edfread.fread('SUB001.EDF', 
										 ignore_samples=True, 
										 properties_filter=['gx', 'pa'])

To see how properties are named check the data2dict function in edfread.pyx and the
edf acces api documentation.

Command line
============

If you have an EDF file with "standard" meta data (e.g. key and value are seperated by a 
blank) you can call

	$> python edf.py SUB001.EDF sub001.dm

The .dm file is a valid hdf5 file that can be read into matlab.


Matlab
======

To read a datamat into matlab do this:

    >> info = h5info('SUB001.dm');
    >> info.Groups.Datasets
    >> field = h5read('SUB001.dm', '/Datamat/fieldname');


