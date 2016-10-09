// readWrite-bmp.cc
//
// extracts pixel data from user-specified .bmp file
// inserts data back into new .bmp file
//
// gw

// uncomment for MSVS
// #include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <omp.h>

#define WIDTH	3
#define HEIGHT	3
#define DEBUG	1
#define KERNEL_SIZE	3

int numThreads = 1;
int threshold = 20;

using namespace std;

#pragma pack(1)
typedef struct {
	char id[2];
	int file_size;
	int reserved;
	int offset;
} header_type;

#pragma pack(1)
typedef struct {
	int header_size;
	int width;
	int height;
	unsigned short int color_planes;
	unsigned short int color_depth;
	unsigned int compression;
	int image_size;
	int xresolution;
	int yresolution;
	int num_colors;
	int num_important_colors;
} information_type;

int globalPrintCount = 0; //this will keep count of the times printArray is called
//Helper function to print an array.
void printImageArray(vector<vector<int> > &image) {
	int imageRows = image.size();
	int imageColsIndex = image[0].size();
	cout << "print time: " << globalPrintCount << endl;
	globalPrintCount++;

	for (int i = 0; i < imageRows; i++) {
		cout << "Row " << i << "[ ";
		for (int j = 0; j < imageColsIndex; j++) {
			cout << image[i][j] << "\t";
		}
		cout << " ]" << endl;
	}
	cout << endl << endl;
}

vector< vector<int> > prepareKernel(int &x, int &y, vector< vector<int> > &image)
{
	int imageRowsIndex = image.size() - 1;
	int imageColsIndex = image[0].size() - 1;

	vector<vector<int> > kernel(KERNEL_SIZE, vector<int>(KERNEL_SIZE, 0));

	if (x > 0 && y > 0 && x < imageRowsIndex && y < imageColsIndex) {
		for (int i = 0; i < KERNEL_SIZE; i++) {
			for (int j = 0; j < KERNEL_SIZE; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	} else if (x == 0 && y > 0 && x < imageRowsIndex && y < imageColsIndex) {
		for (int i = 1; i < KERNEL_SIZE; i++) {
			for (int j = 0; j < KERNEL_SIZE; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	} else if (x == 0 && y == 0 && x < imageRowsIndex && y < imageColsIndex) {
		for (int i = 1; i < KERNEL_SIZE; i++) {
			for (int j = 1; j < KERNEL_SIZE; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	} else if (x > 0 && y == 0 && x < imageRowsIndex && y < imageColsIndex) {
		for (int i = 0; i < KERNEL_SIZE; i++) {
			for (int j = 1; j < KERNEL_SIZE; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	} else if (x == imageRowsIndex && y < imageColsIndex) {
		for (int i = 0; i < KERNEL_SIZE - 1; i++) {
			for (int j = 0; j < KERNEL_SIZE; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	} else if (x == imageRowsIndex && y == 0) {
		for (int i = 0; i < KERNEL_SIZE - 1; i++) {
			for (int j = 1; j < KERNEL_SIZE; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	} else if (x == imageRowsIndex && y == imageColsIndex) {
		for (int i = 0; i < KERNEL_SIZE - 1; i++) {
			for (int j = 0; j < KERNEL_SIZE - 1; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	} else if (x == 0 && y == imageColsIndex) {
		for (int i = 1; i < KERNEL_SIZE; i++) {
			for (int j = 0; j < KERNEL_SIZE - 1; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	} else if (x < imageRowsIndex && y == imageColsIndex) {
		for (int i = 0; i < KERNEL_SIZE; i++) {
			for (int j = 0; j < KERNEL_SIZE - 1; j++) {
				kernel[i][j] = image[(x - 1) + i][(y - 1) + j];
			}
		}
	}

	return kernel;

}

int sobelFilter (int x, int y, vector< vector<int> > &image)
{
	//prepare data;
	vector<vector<int> > dataToFilter = prepareKernel(x, y, image);

	// printImageArray(dataToFilter);

	//Sobel's algorithm is quite simple, we just add
	//the derivatives in x and y (called gradient magnitude)
	int x0, x1, x2, x3, x5, x6, x7, x8;
	x0 = dataToFilter[0][0];
	x1 = dataToFilter[0][1];
	x2 = dataToFilter[0][2];
	x3 = dataToFilter[1][0];
	x5 = dataToFilter[1][2];
	x6 = dataToFilter[2][0];
	x7 = dataToFilter[2][1];
	x8 = dataToFilter[2][2];

	int dfdy = (x0 + 2 * x1 + x2) - (x6 + 2 * x7 + x8);
	int dfdx = (x2 + 2 * x5 + x8) - (x0 + 2 * x3 + x6);

	int gradient = abs(dfdy) + abs(dfdx);

	//the actual filter:
	if (gradient < threshold) {
		return 0;
	} else {
		return 255;
	}
}

header_type header;
information_type information;
string imageFileName, newImageFileName;
unsigned char tempData[3];
int row, col, row_bytes, padding;
vector <vector <int> > data, newData;
ifstream imageFile;
ofstream newImageFile;

struct index_limits {
    int lower_index;
    int higher_index;
};

void *ProcessData(void *arg) {
	struct index_limits *limits = (struct index_limits *)arg;

	cout << "test for receiving an argument Limits: lower" << limits->lower_index << " higher: ";
	cout << limits->higher_index <<  endl;
	// read/compute image information
	imageFile.read ((char *) &information, sizeof(information_type));
	row_bytes = information.width * 3;
	padding = row_bytes % 4;
	if (padding)
		padding = 4 - padding;

	// extract image data, initialize vectors
	for (row=0; row < information.height; row++) {
		data.push_back (vector <int>());
		for (col=0; col < information.width; col++) {
			imageFile.read ((char *) tempData, 3 * sizeof(unsigned char));
			data[row].push_back ((int) tempData[0]);
		}
		if (padding)
			imageFile.read ((char *) tempData, padding * sizeof(unsigned char));
	}
	cout << imageFileName << ": " << information.width << " x " << information.height << endl;


	// pixel info is now stored in the 2D vector called 'data'
	// at this point you could, for example, print it to a file
	// your code (in any language) could then perform the image transformation
	// the transformed data would need to be re-inserted into an image-formatted file for display

	// if you with to use this C++ skeleton program for image wrangling,
	// insert your transformation code here...

	// this loop shows how to simply recreate the original Black-and-White image
	for (row=0; row < information.height; row++) {
		newData.push_back (vector <int>());
		for (col=0; col < information.width; col++) {
			newData[row].push_back (/*data[row][col]*/sobelFilter(row,col,data));
		}
	}

	// write header to new image file
	newImageFile.write ((char *) &header, sizeof(header_type));
	newImageFile.write ((char *) &information, sizeof(information_type));

	// write new image data to new image file
	for (row=0; row < information.height; row++) {
		for (col=0; col < information.width; col++) {
			tempData[0] = (unsigned char) newData[row][col];
			tempData[1] = (unsigned char) newData[row][col];
			tempData[2] = (unsigned char) newData[row][col];
			newImageFile.write ((char *) tempData, 3 * sizeof(unsigned char));
		}
		if (padding) {
			tempData[0] = 0;
			tempData[1] = 0;
			tempData[2] = 0;
			newImageFile.write ((char *) tempData, padding * sizeof(unsigned char));
		}
	}
	cout << newImageFileName << " done." << endl;
	imageFile.close();
	newImageFile.close();
}

int main(int argc, char* argv[])
{
	string strThreshold, strNumThreads;
	// prepare files
	cout << "Original imagefile? ";
	cin >> imageFileName;
	imageFile.open(imageFileName.c_str(), ios::binary);
	if (!imageFile) {
		cerr << "file not found" << endl;
		exit(-1);
	}
	cout << "New imagefile name? ";
	cin >> newImageFileName;
	newImageFile.open(newImageFileName.c_str(), ios::binary);

	// read file header
	imageFile.read((char *) &header, sizeof(header_type));
	if (header.id[0] != 'B' || header.id[1] != 'M') {
		cerr << "Does not appear to be a .bmp file.  Goodbye." << endl;
		exit(-1);
	}

	//Request threshold
	cout << "Threshold? ";
	cin >> strThreshold;
	threshold = atoi(strThreshold.c_str());

	//Request number of threads
	cout << "Number of threads? ";
	cin >> strNumThreads;
	numThreads = atoi(strNumThreads.c_str());

	pthread_t threads[numThreads];
	  int rc;
	  long t;
	  for(t=0; t<numThreads; t++){
	    printf("In main: creating thread %ld\n", t);
	    struct index_limits args;
	    args.lower_index = 10;
	    args.higher_index = 15;

	    rc = pthread_create(&threads[t], NULL, ProcessData, (void *)&args);
	    if (rc){
	      printf("ERROR; return code from pthread_create() is %d\n", rc);
	      exit(-1);
	    }
	  }

	  /* Last thing that main() should do */
	  pthread_exit(NULL);

	return 0;
}
