# pyedfread


A utility that parses SR research EDF data files into pandas DataFrames.
This package was initially developed by [Niklas Wilming](https://github.com/nwilming/) and updated by [Neal Morton](https://github.com/mortonne/) under the name of `edfread`. Now it is maintained by the [Computational Cognitive Science Stuttgart](www.s-ccs.de) group with official permission from Wilming & Morton.

## Requirements


EyeLink Developers Kit. Download from [SR-Research support forum](https://www.sr-research.com/support/thread-13.html)
 (forum registration required).

 > I do not include the SR Research EDF Access API header files and libraries.
 > These are needed to compile pyedfread with cython. If you use a mac you can
 > download a package from the SR-Research support forum. If you use Ubuntu you
 > can install them via apt-get.
 

## Alternatives
Due to the "unmaintained" status of this repo ca. 2019-2024 some alternatives came up:

[eyelinkio](https://github.com/scott-huberty/eyelinkio)

If you are willing to run `edf2asc` on all your files, you can use:

[mne-python](https://mne.tools/stable/auto_tutorials/io/70_reading_eyetracking_data.html)

[ParseEyeLinkAscFiles](https://github.com/djangraw/ParseEyeLinkAscFiles)

[eyelinkparser](https://github.com/open-cogsci/eyelinkparser)

## Contributors
- Niklas Wilming
- Selim Onat
- Chadwick Boulay
- Niel Morton
- Andrea Constantino
- Benedikt Ehinger (maintainer)
- Alex Kim
- Greg Schwimer
- Oscar Esteban
- Richard Veale

## Setup

Run  `pip install git+https://github.com/s-ccs/pyedfread` to compile and install. This will install the
python library and a command line script to parse edfs.


### Windows Support
As of this writing (July 2019), the EyeLink Developers Kit for Windows requires small modifications
before it will work with this project. Administrator access is required to edit the files
in their default directory: C:\Program Files (x86)\SR Research\EyeLink\Includes\eyelink/

Edit edftypes.h. Replace the chunk of typedefs (lines 40-49) with the following:
```C
#ifndef BYTEDEF
	#define BYTEDEF 1
	typedef unsigned char  byte;
	#ifndef _BASETSD_H_ /* windows header */
		typedef short          INT16;
		typedef int            INT32;
		typedef unsigned short UINT16;
		typedef unsigned int   UINT32;
	#endif
#endif
```

Some user reported the following, but in 2024 BenediktEhinger didnt need this:

One of the DLLs from SR Research (zlibwapi.dll) depends on MSVCP90.dll.
This _should_ come with MS Visual Studio C++ 2008 SP1 redistributable, but its installer didn't seem to put the file on the PATH.
Instead, I had already installed [Mercurial for Windows](https://www.mercurial-scm.org/release/windows/mercurial-4.9.1-x64.msi)
which comes with the correct version of that file and puts it on the PATH by default.

 
## Usage

pyedfread can be used on the command line (convert_edf) or called from
within python.

## From python

After compilation run the following lines for a quick test.

    >>> import pyedfread
    >>> samples, events, messages = pyedfread.read_edf('SUB001.EDF')

This opens SUB001.EDF and parses it three DataFrames:

 - samples contain individual samples.
 - events contains fixation and saccade definitions
 - messages contains meta data associated with each trial.

pyedfread allows to select which meta data you want to read from your edf file.
This happens through the 'filter' argument of edf.pread / pyedfread.fread. It can
contain a list of 'message' identifiers. A message identifier in the EDF is
trial metadata injected into the data stream during eye tracking. If
for example after the start of a trial you send the message "condition 1", you
can add 'condition' to the filter list and pyedfread will automagically add a
field condition with value 1 for each trial to the messages structure. Of course,
if the value varies across trials this will be reflected in the messages
structure. This is what it looks like in python code:

	>>> samples, events, messages = edf.read_edf('SUB001.EDF', ignore_samples=True, message_filter=['condition'])

If the filter is not specified, pyedfread saves all messages it can parse.

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


## Command line


If you have an EDF file with "standard" meta data (e.g. key and value are seperated by a
blank) you can call

	$> convert_edf SUB001.EDF sub001.hdf

The .hdf file is a valid hdf5 file that can be read into matlab. The default is
to simplify the HDF file by replacing strings with numbers. The original strings
are saved as attributes in the HDF file. Fields that contain python objects (e.g.
arrays, lists etc.) are skipped.

Run 'read_edf -h' for some help.


## Matlab


To read a datamat into matlab do this:

    >> info = h5info('SUB001.hdf');
    >> info.Groups.Datasets
    >> field = h5read('SUB001.hdf', '/events/gavx');


## Testing
While there are some basic unit-testing setup, it is currently not possible to use continuous integration, as we cannot provide the required SR-Research libraries.

if you want to run the test use:

`tox -e py`

after cloning

## License


BSD License, see LICENSE file
