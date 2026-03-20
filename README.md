# WUSTL_Sortcode

WUSTL sort code for use with Gobbi array, TexNeut array, and diamond detector for measurement of 6Li(2+2)->α+p+n.
Code adapted Nicolas Dronchi, who adapted his code from Robert Charity and Kyle Brown.

Writen by Henry Webb (h.s.webb@wustl.edu)

# Instructions

To set up project:

1. Run `git clone <URL>`, where `<URL>` should be replaced with the desired URL of the project's GitHub repository
2. Run `cd li6plus2sort` to enter the project directory
3. Run `git submodule update --init --recursive` inside the project's root directory. This initializes all of the project's git submodules
4. Proceed to the build steps below

To build:

1. In root project directory, run `mkdir build`
2. Then `cd build`
3. Then `cmake ..`
4. Finally, `make -j`
5. Run the code with `./sort` inside the build directory

Before running the code, make sure to change all of the configuration settings in `sort.config` and `tnlib.config` to the desired values, along with all the other various required input files for the different parts of the code.

# TexNeut input file details

barmap.txt column ordering:
	- Each PMT gets its own line in this file
	- `inchip` and `inchipchan` are the PMT chip and channel assigned to each particular PMT

inchip | inchipchan | outchan | outtdcchan | outx | outy | outbarnum | outAoffset | outBoffset


# Old instructions from Nic

zlines:
To draw zlines, open sort.root, navigate to the DeltaE-E plots (DEEplots/DEE0).
Run the following lines in the root command line:

.L zline/banana.C
banana()

//here you need to click on the TBrowser interface to draw the gates

//Copy paste the data from banana.dat into a .zline file for the particle identified

.L zline/readPIDzline.C
readPIDzline(1)
//where 1 is the quadrant number of interest


