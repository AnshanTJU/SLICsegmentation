/*
 * SLIC Segmentation
 *
 * Author:  Radhakrishna Achanta
 * Modified by: Shan An (anshan.tju@gmail.com) Dec 16, 2012
 * 
 * The original code is in http://ivrgwww.epfl.ch/supplementary_material/RK_SLICSuperpixels/index.html
 * Modification: The original code use windows GDI+ to handle images. This package is a linux version 
 * using libjpeg and libpng to read and write JPEG/PNG images.
 */

SLIC class declaration and implementation files are provided. 
The files provide the code to perform superpixel segmentation as explained in the paper:

"SLIC Superpixels", Radhakrishna Achanta, Appu Shaji, Kevin Smith, Aurelien Lucchi, Pascal Fua, and Sabine Susstrunk.
EPFL Technical Report no. 149300, June 2010.

If you use SLIC Superpixels, you should cite Radhakrishna Achanta's paper. 


INSTALLATION AND DEPENDENCIES
-----------------------------
This code requires libpng and libjpeg.

I have tested this software with Fedora 10.

The test image "6046.jpg" is downloaded from The Berkeley Segmentation Dataset and Benchmark:
http://www.eecs.berkeley.edu/Research/Projects/CS/vision/bsds/

Another image "image1.png" is downloaded from Autostitch
http://www.cs.bath.ac.uk/brown/autostitch/autostitch.html

Please let me know if you find any issues with this code.

