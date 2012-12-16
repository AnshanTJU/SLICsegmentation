/**
 * @file SLICsegmentation.cpp
 * @brief Simple Linear Interative clustering algorithm
 *
 * Usage: 
 * 1. For an image
 *    ./SLICsegmentation -f infilename(.jpg/.png) -o outfiledirectory 
		-k desired_superpixel_number -m compactness_value
 * 2. For a directory contains images.
 *    ./SLICsegmentation -r infiledirectory -o outfiledirectory -m format(.jpg/.png) 
		-k desired_superpixel_number -m compactness_value
 *
 * Parameters:
 *
 * input image (directory) (default:.jpg)
 * output image (directory)
 * desired_superpixel_number (default: 200)
 * compactness_value (default: 10.0)
 *
 * Read/write operations (jpeg format) make use
 * of io_jpeg.c and io_jpeg.h

 * @author Shan An <anshan.tju@gmail.com>
 *
 */

/**
 * @mainpage SLIC Segmentation
 *
 * README.txt:
 * @verbinclude README.txt
 */


/**
 * @file   SLICsegmentation.cpp
 * @brief  Main executable file
 *
 *
 * @author Shan An <anshan.tju@gmail.com>
 */

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>

#include "image_io/io_jpeg.h"
#include "image_io/io_png.h"
#include "SLIC.h"

using namespace std;

//Globle variables
int spcount = 200;	//desired_superpixel_number
double compactness = 10.0;	//compactness_value
string infileDir = "./";
string outfileDir = "./";
string infileName = "infile.jpg";
string outfileName = "outfile.jpg";
string img_format = ".jpg";
bool single_image = true; //true if only has one input image
long tv_usec = 0;	//microsecond

/**
 * @brief Parse argments
 */
int parseCmdArgs(int argc, const char** argv)
{
	int i;
	if (argc < 3) {
        cout<<
		"Usage: ./SLICsegmentation -f infilename(.jpg/.png) -o outfiledirectory \
[-k] [desired_superpixel_number=200] [-m] [compactness_value=10.0] \n"
		"OR \n"
		"Usage: ./SLICsegmentation -r infiledirectory -o outfiledirectory -m format(.jpg/.png) \
[-k] [desired_superpixel_number=200] [-m] [compactness_value=10.0] \n";
        return EXIT_FAILURE;
	}
	// Parse command args
	for (i = 1; i < argc; ++i)
    	{
		if (string(argv[i]) == "-f") 
		{
			infileName = argv[i + 1];
			i++; 
			single_image = true;
		}
		else if (string(argv[i]) == "-r")
		{
			infileDir = argv[i + 1];
			i++; 
			single_image = false;
   		}
		else if (string(argv[i]) == "-o") 
		{
			outfileDir = argv[i + 1];
			i++; 
		}
		else if (string(argv[i]) == "-m") 
		{
			img_format = argv[i + 1];
			i++; 
		}
		else if (string(argv[i]) == "-k")
		{
			spcount = atoi(argv[i + 1]);
			i++;
		}
		else if (string(argv[i]) == "-m")
		{
			compactness = atof(argv[i + 1]);
        		i++;
		}
	}
	return 0;
}

/**
 * @brief Get pictures from infilename or infiledirectory
 */
vector<string> getPictures(const char *path, const char *right, bool sorted)
{
	DIR *dirp;
	struct dirent *entry;
	vector<string> ret;
	if((dirp = opendir(path)) != NULL){
		while((entry = readdir(dirp)) != NULL){
			string name(entry->d_name);
			string r(right);
			if((name.length() >= r.length()) && 
				(name.substr(name.length() - r.length()).compare(r) == 0))
			{
				ret.push_back(string(path) + "/" + entry->d_name);
			}
		}
		closedir(dirp);
	}
	if(sorted) sort(ret.begin(), ret.end());
	return ret;
}

/**
 * @brief Converts input color data from format RRR...GGG...BBB to RGBRGB...
 */
unsigned char *convertInputData(unsigned char *input, int size)
{
	unsigned char *data = NULL;
	data = (unsigned char*)malloc(3*size*sizeof(unsigned char));
	int n;
    for (n = 0; n < size; n++) {
		data[3*n] = input[n];
		data[3*n + 1] = input[size + n];
        	data[3*n + 2]= input[2*size + n];
    	} 	return data;
}

/**
 * @brief Converts output color data from format RGBRGB... to RRR...GGG...BBB
 */
unsigned char *convertOutputData(unsigned char *input, int size)
{
	unsigned char *data = NULL;
	data = (unsigned char*)malloc(3*size*sizeof(unsigned char));
	int n;
    for (n = 0; n < size; n++) {
		data[n] = input[3*n];
		data[size + n] = input[3*n + 1];
        	data[2*size + n]= input[3*n + 2];
    	} 	return data;
}

int main(int argc, const char *argv[]) 
{
	int k = 0;
	unsigned char *img = NULL;
	unsigned char *imgtmp = NULL;
	int* labels = NULL;
	int width = 0; int height = 0; 
	int bytes_per_pixel = 0; int color_space = 0;

	int retval = parseCmdArgs(argc, argv);
    if (retval)
        return retval;

	vector<string> picvec(0);
	if (single_image)
	{
		picvec.push_back(infileName);
		img_format = infileName.substr(picvec[k].size() - 4, picvec[k].size()); 
	}
	else
	{
		picvec = getPictures(infileDir.c_str(), img_format.c_str(), true);
	}
		
	int numPics = picvec.size() ;
	string path;
	int sz = 0;  int numlabels = 0;
	struct timeval start, end;
  	double timeuse = 0.0; double totaltime = 0.0;
	for( k = 0; k < numPics; k++ )
	{
		img = NULL;
		labels = NULL;
		path = outfileDir;
		//read image
		if (img_format == ".jpg")
		{
			img = read_jpeg_u8(picvec[k].c_str(), &width, &height, &bytes_per_pixel, &color_space);
		}
		else if(img_format == ".png")
		{
			size_t src_w, src_h;
			imgtmp = read_png_u8_rgb(picvec[k].c_str(), &src_w, &src_h);
			width = (int)src_w;
			height = (int)src_h;
			//Converts color data from format RRR...GGG...BBB to RGBRGB...
			//because in "io_png.c" png RGB RGB RGB 8bit changed to RRR GGG BBB 
			img = convertInputData(imgtmp, width*height);
		}
		else
		{
			printf("Unrecognised image format, only support JPEG/PNG! \n");
			return EXIT_FAILURE;
		}
		
		sz = width*height;
		//---------------------------------------------------------
		if(spcount < 20 || spcount > sz/4) spcount = sz/200;//i.e the default size of the superpixel is 200 pixels
		if(compactness < 1.0 || compactness > 80.0) compactness = 10.0;
		//---------------------------------------------------------
		labels = new int[sz];
		numlabels = 0;
		// start timer
		gettimeofday( &start, NULL );
		SLIC slic;
		slic.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(img, width, height, labels, numlabels, spcount, compactness);
		//slic.DoSuperpixelSegmentation_ForGivenSuperpixelSize(img, width, height, labels, numlabels, 10, compactness);//demo
		// calculate time
		gettimeofday( &end, NULL );
		timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
		timeuse /= 1000000;
		totaltime += timeuse;

		slic.DrawContoursAroundSegments(img, labels, width, height, 0);

		path += picvec[k].substr(0, picvec[k].size() - 4); 
		path.append("_SLIC"); path.append(img_format);
		//output result image
		if (img_format == ".jpg")
		{
			write_jpeg_u8(path.c_str(), img, width, height, bytes_per_pixel, color_space);
		}
		else if(img_format == ".png")
		{
			imgtmp = convertOutputData(img, width*height);
			write_png_u8(path.c_str(), imgtmp, width, height, 3);
		}
		else
		{
			return EXIT_FAILURE;
		}
	}
	if(labels) delete [] labels;
	if(img) delete [] img;
	if(imgtmp) delete [] imgtmp;	
	printf("Time used: %.6f s; Average time:%.6f s \n", totaltime, totaltime/numPics);
	printf("Done! \n");
	return 0;
}
