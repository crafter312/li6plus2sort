# WUSTL_Sortcode

WUSTL sort code for use with Gobi Array in search for Li7 excited states
Code adapted from Robert Charity and Kyle Brown

Writen by Nicolas Dronchi






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


