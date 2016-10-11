// This is a version for trial and error, it has a lot of commented code
// (some working). I'm creating this copy for experiments, there's no make
// for this file, or associated with it. ENJOY :)
//
// not including this in the make
// readWrite-bmp-sketchboard.cc
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

#define WIDTH 3
#define HEIGHT 3
#define DEBUG 1

using namespace std;

#pragma pack(1)
typedef struct {
	char id[2];
	int file_size;
	int reserved;
	int offset;
}  header_type;

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

// float filter[WIDTH][HEIGHT];

// int result(int x, int y, vector <vector <int> > &image)
// {
// 	int n = WIDTH; //size of the filter
// 	int result = 0;
// 	// if( x == 0 || y == 0 || x == image.size() || y == image[0].size()){
// 	// 	int paddedRowSize = image.size()+2;
// 	// 	int paddedColSize = image[0].size()+2;
// 	// 	vector< vector <int> > paddedImage (paddedRowSize, vector<int>(paddedColSize));
// 	// 	for (int i = 0; i < paddedRowSize; i++)
// 	// 	{
// 	// 		for (int j = 0; j < paddedColSize ; j++)
// 	// 		{
// 	// 			if(i == 0)
// 	// 			{
// 	// 				paddedImage[i][j] = 0;
// 	// 			}
// 	// 			if(j == 0)
// 	// 			{
// 	// 				paddedImage[i][j] = 0;
// 	// 			}
// 	// 		}
// 	// 	}
// 	// }
// 	int newX = 0, newY = 0;
// 	int imageWidth = image.size();
// 	int imageHeight = image[0].size();
//
// 	for (int i = 0; i < n; i++)
// 	{
// 		newX = (x-1) + i;
// 		for (int j = 0; j < n; j++)
// 		{
//
// 			// float imageX = x-floor(n/2)+i;
// 			// int newX = (imageX >= 0) ? (int)(imageX + 0.5) : (int)(imageX - 0.5);
// 			// float imageY = y-floor(n/2)+j;
// 			// int newY = (imageY >= 0) ? (int)(imageY + 0.5) : (int)(imageY - 0.5);
// 			newY = (y-1) + j;
// 			cout << "x" << newX << "y" << newY << endl;
// 			if ((newX >= 0 && newY >= 0) && (newX < imageWidth && newY < imageHeight)){
// 				result += filter[i][j]*image[newX][newY];
// 				if (DEBUG)
// 				{
// 					cout << "filter[" << i <<"]["<< j <<"] = " << filter[i][j] <<endl;
// 					cout << "image["<<x<<"-("<<n<<"/2)+"<<i<<" = "<< newX << "]["<< y << "-("<<n<<"/2)+";
// 					cout << j <<" = "<< newY << "] = "<< image[newX][newY];
// 					cout << "Partial Result = " << result << endl;
// 				}
// 			}
// 		}
// 	}
// 	cout << "Total Result = " << result << endl;
// 	return result;
// }
int globalPrintCount = 0;
void printImageArray(vector< vector<int> > &image)
{
	int imageRows = image.size();
	int imageColsIndex = image[0].size();
	cout << "print time: " << globalPrintCount << endl;
	globalPrintCount++;

	for (int i = 0; i < imageRows; i ++){
		cout << "Row "<< i << "[ ";
		for (int j = 0; j < imageColsIndex; j++){
			cout << image[i][j] << "\t";
		}
		cout << " ]" << endl;
	}
	cout << endl << endl;
}

#define KERNEL_SIZE 3
vector< vector<int> > prepareKernel(int &x, int &y, vector< vector<int> > &image)
{
	int imageRowsIndex = image.size() - 1;
	int imageColsIndex = image[0].size() - 1;

	vector< vector<int> > kernel (KERNEL_SIZE, vector<int>(KERNEL_SIZE,0));

	if (x > 0 && y > 0 && x < imageRowsIndex && y < imageColsIndex)
	{
		for(int i = 0; i < KERNEL_SIZE; i++ )
		{
			for (int j=0; j < KERNEL_SIZE; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	} else if (x == 0 && y > 0 && x < imageRowsIndex && y < imageColsIndex)
	{
		for(int i = 1; i < KERNEL_SIZE; i++ )
		{
			for (int j=0; j < KERNEL_SIZE; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	} else if (x == 0 && y == 0 && x < imageRowsIndex && y < imageColsIndex)
	{
		for(int i = 1; i < KERNEL_SIZE; i++ )
		{
			for (int j=1; j < KERNEL_SIZE; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	} else if (x > 0 && y == 0 && x < imageRowsIndex && y < imageColsIndex)
	{
		for(int i = 0; i < KERNEL_SIZE; i++ )
		{
			for (int j=1; j < KERNEL_SIZE; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	} else if (x == imageRowsIndex && y < imageColsIndex)
	{
		for(int i = 0; i < KERNEL_SIZE-1; i++ )
		{
			for (int j=0; j < KERNEL_SIZE; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	} else if (x == imageRowsIndex && y == 0)
	{
		for(int i = 0; i < KERNEL_SIZE-1; i++ )
		{
			for (int j=1; j < KERNEL_SIZE; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	} else if (x == imageRowsIndex && y == imageColsIndex)
	{
		for(int i = 0; i < KERNEL_SIZE-1; i++ )
		{
			for (int j=0; j < KERNEL_SIZE-1; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	} else if (x == 0 && y == imageColsIndex)
	{
		for(int i = 1; i < KERNEL_SIZE; i++ )
		{
			for (int j=0; j < KERNEL_SIZE-1; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	} else if (x < imageRowsIndex && y == imageColsIndex)
	{
		for(int i = 0; i < KERNEL_SIZE; i++ )
		{
			for (int j=0; j < KERNEL_SIZE-1; j++)
			{
				kernel[i][j] = image[(x-1)+i][(y-1)+j];
			}
		}
	}

	return kernel;

}

int threshold = 20;
int sobelFilter (int x, int y, vector< vector<int> > &image)
{
	//prepare data;
	vector< vector<int> > dataToFilter = prepareKernel(x,y, image);

	// printImageArray(dataToFilter);

	//Sobel's algorithm is quite simple, we just add
	//the derivatives in x and y (called gradient magnitude)
	int x0,x1,x2,x3,x5,x6,x7,x8;
	x0 = dataToFilter[0][0];
	x1 = dataToFilter[0][1];
	x2 = dataToFilter[0][2];
	x3 = dataToFilter[1][0];
	x5 = dataToFilter[1][2];
	x6 = dataToFilter[2][0];
	x7 = dataToFilter[2][1];
	x8 = dataToFilter[2][2];

	int dfdy = (x0 + 2*x1 +x2) - (x6 + 2*x7 + x8);
	int dfdx = (x2 + 2*x5 +x8) - (x0 + 2*x3 + x6);

	int gradient = abs(dfdy) + abs(dfdx);

	//the actual filter:
	if(gradient < threshold)
	{
		return 0;
	} else {
		return 255;
	}
}

int main(int argc, char* argv[])
{
	header_type header;
	information_type information;
	string imageFileName, newImageFileName, strThreshold;
	unsigned char tempData[3];
	int row, col, row_bytes, padding;
	vector <vector <int> > data, newData;

	// prepare files
	cout << "Original imagefile? ";
	cin >> imageFileName;
	ifstream imageFile;
	imageFile.open (imageFileName.c_str(), ios::binary);
	if (!imageFile) {
		cerr << "file not found" << endl;
		exit(-1);
	}
	cout << "New imagefile name? ";
	cin >> newImageFileName;
	ofstream newImageFile;
	newImageFile.open (newImageFileName.c_str(), ios::binary);

	// read file header
	imageFile.read ((char *) &header, sizeof(header_type));
	if (header.id[0] != 'B' || header.id[1] != 'M') {
		cerr << "Does not appear to be a .bmp file.  Goodbye." << endl;
		exit(-1);
	}

	//ask for threshold
	cout << "threshold? ";
	cin >> strThreshold;
	threshold = atoi(strThreshold.c_str());

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

	//for testing purposes:
	// vector< vector<int> > testArray(5,vector<int>(5));
	// testArray[0][0] = 241;
	// testArray[0][1] = 35;
	// testArray[0][2] = 19;
	// testArray[0][3] = 13;
	// testArray[0][4] = 25;
	//
	// testArray[1][0] = 18;
	// testArray[1][1] = 214;
	// testArray[1][2] = 74;
	// testArray[1][3] = 82;
	// testArray[1][4] = 44;
	//
	// testArray[2][0] = 73;
	// testArray[2][1] = 41;
	// testArray[2][2] = 241;
	// testArray[2][3] = 87;
	// testArray[2][4] = 33;
	//
	// testArray[3][0] = 35;
	// testArray[3][1] = 79;
	// testArray[3][2] = 66;
	// testArray[3][3] = 219;
	// testArray[3][4] = 60;
	//
	// testArray[4][0] = 13;
	// testArray[4][1] = 12;
	// testArray[4][2] = 62;
	// testArray[4][3] = 53;
	// testArray[4][4] = 236;

	// filter[0][0] = -0.0625;
	// filter[0][1] = -0.0625;
	// filter[0][2] = -0.0625;
	//
	// filter[1][0] = -0.0625;
	// filter[1][1] =  0.5;
	// filter[1][2] = -0.0625;
	//
	// filter[2][0] = -0.0625;
	// filter[2][1] = -0.0625;
	// filter[2][2] = -0.0625;

	// for (int i = 0; i < 5; i ++){
	// 	cout << "Row "<< i << "[ ";
	// 	for (int j = 0; j < 5; j++){
	// 		// sobelFilter(i,j,testArray);
	// 		cout << /*result(i,j,testArray)*/ sobelFilter(i,j,testArray) << ",";
	// 	}
	// 	cout << "]" << endl;
	// }


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

	return 0;
}
