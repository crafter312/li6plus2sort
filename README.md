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

# TexNeut analysis macros

There are a number of macros which live inside the **macros/** subdirectory. Here is a list of the important ones in the order that they should be used during an analysis, along with what each one does:
0. **texneut_only_sort/**: This subdirectory contains a sort code for processing data containing *only* TexNeut data. This is useful for, say, 137Cs calibration data. 
1. **TNGainMatch.C**: This macro should be run first. It should be run on 137Cs or similar calibration data. This is important so that the events primarily populate the linear low energy region. This macro is used to extract a scaling value to gain match the top and bottom PMTs by converting the finger plot to polar coordinates and then essentially symmetrizing the polar angle about 45 degrees.
2. **TNGainFileConvert.C**: This macro converts the output of the **TNGainMatch.C** code to a format closer to that which is used by the main sort code.
3. **TNCrystalGates.C**: This macro is used to mark a saturation cutoff and draw lines around the finger plot (one down the middle and two around the sides). These lines are used by the sort code to calculate a parameter which one can use to more effectively separate the crystals from each other, call it `Lnorm`. This macro also contains a bunch of code which I used to test this initial calculation procedure, but which is now commented out. I also am aware that the name of this macro is confusing, it has made it difficult for me to remember exactly what file does this task.
4. **PSDSetEThresholds.C**: This macro is used to set software thresholds for all the bars. I do this using the 137Cs data, since in theory this should have plenty of statistics in all of the detectors. The way this works is it plots Lnorm (the value calculated using the finger plot outlines from **TNCrystalGates.C**) vs. the total energy. Then, one can click inside the histogram to place a line, and adjust the line until visually it seems to be in a good spot. The code saves images of all the plots with the lines for future reference, and also outputs all of the results to a file.
5. **find_bar_gates.C**: After drawing lines around the finger plots and running the sort code with the results to calculate the new coordinates (primarily `Lnorm`), this macro can then be run to fit a six peak Gaussian fit to each bar's `Lnorm` distribution and calculate an ideal point of separation for each pair of neighboring crystals using the centroids and widths of the peaks for those crystals. The thresholds are used to remove background and improve the fit quality.
6. **PSDComptonCal.C**: Now that one has the ability to extract the crystal ID of each event, and after running the 137Cs data or similar again through the sort code to get the newest results, one should run this macro to calibrate each crystal's E_tot using the 137Cs Compton edge. Obviously, if one used a different source, one should change the Compton edge value in the macro used for the calibration.
7. **time_calibration_PSD.C**: This macro is used to perform a time calibration of the PSD times. The TDC times likely do not need this, because they are already calibrated (at least from the TDC we used). The data should be of the standard format "put in a 10 or 20 ns cable half way into the run," such that one has two peaks in the time spectrum that are separated in time by the length of the cable used.
8. **TDC_Eshift_optimize.C**: This macro is used to calculate an energy offset (i.e. from the integral offsets in the PSD system) per bar by calculating a weighted average of the top and bottom PMT's time values for a bunch of applied arbitrary energy shifts and then looking for the energy shift that minimizes the time resolution of the prompt gamma peak.
9. **MACRO NOT MADE YET**: This macro is used to calculate time shifts for every crystal to line up the prompt gamma peaks with the expected time of flight for gammas given the distance between the crystal and the center of the target.
10. **DrawPSDGates.C**: Finally, this macro is used to help draw neutron PSD gates for every crystal.

Wow, you might say, that's a lot of macros! And you'd be right. Maybe there's a better way to do this, idk. But by the time you get through all these steps, you'll have some nice, calibrated TexNeut data with extracted position data, shifted and resolution optimized time information, and neutron/gamma separation. Then you can use this data to, idk, calculate invariant mass or something like that.

# TexNeut input file details

barmap.txt column ordering:
* Each PMT gets its own line in this file
* `inchip` and `inchipchan` are the PMT chip and channel numbers assigned to each particular PMT
* `outchan` is an arbitrary, unique identifying number assigned to each PMT
* `outtdcchan` is the TDC channel assigned to the PSD board attributed to each PMT
* `outx` is a unitless position index, the axis of which is horizontal and along the 16 bar/PMT wide side of TexNeut
* `outy` is a unitless position index, the axis of which is along the remaining horizontal dimension of TexNeut
* `outbarnum` marks which scintillating bar each PMT is attached two. There should always and only ever be two PMTs with the same `outbarnum`
* `outAoffset` and `outBoffset` are constant offsets subtracted from each integral value, after scaling by the gain values

```
inchip | inchipchan | outchan | outtdcchan | outx | outy | outbarnum | outAoffset | outBoffset
```

pmtgains.txt column ordering:
* Each scintillating bar gets its own line in this file
* `bar` is the ID number of the scintillating bar, which should match `outbarnum` in `barmap.txt`
* `pmttop` and `pmtbottom` are the IDs of the top and bottom PMTs for the given scintillating bar, which should match `outchan` for each PMT in `barmap.txt`
* `topgain` and `botgain` are the scaling values for each PMT's integrals, read into the code as double precision floating point numbers

```
bar | pmttop | topgain | pmtbot | botgain
```

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


