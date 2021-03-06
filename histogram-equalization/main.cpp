/**
 *
 * Copyright (C) 2019
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @file main.cpp
 *
 * @brief Does histogram equalization
 * Histogram equalization which are mentioned at Chapter 3.3 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
 *
 * @author Ozan Cansel
 * Contact: ozancansel@gmail.com
 * 
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "utility.h"

#define L 256

using namespace cv;

//Fills the array with zeros
void fillZero(double* arr, int size);

/* Chapter 3.3 is implemented. It performs 3.3-8. equation on the image.
* It is also useful to look at 
* https://www.tutorialspoint.com/dip/introduction_to_probability.htm and https://www.tutorialspoint.com/dip/histogram_equalization.htm
* pages for simpler explanation.
*/
Mat equalizeHistogram(Mat input);

void fillZero(double* arr, int size);

double* calculateHistogram(Mat input);

//pdf array  represents the pr(rj) part of Eq. 3.3-8
double* calculatePdf(double* histogram, int totalPixelSize);

//                          k
//Cdf function reprents the E pr(rj) part of Eq. 3.3-8
//                          j=0
double* calculateCdf(double* pdf);

int main(int argc, char** argv) {
        const String keys = 
	"{help h usage ?    || The program does histogram equalization on the image.}"
    "{input             | histogram-equalization.png | input image}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

	Mat input;

    input = imread(cmdParser.get<cv::String>("input").c_str());

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

    cvtColor(input , input , COLOR_BGR2GRAY);

	//Performs histogram equalization
	auto histogramEqualizedImage = equalizeHistogram(input);

    imshow("input" , input);
	imshow("Histogram equalized output", histogramEqualizedImage);

    waitKey(0);

    return 0;
}

void fillZero(double* arr, int size)
{
	for (auto i = 0; i < size; ++i)
	{
		arr[i] = 0;
	}
}

double* calculateHistogram(Mat input)
{
	double* inputHistogram = new double[L];
	fillZero(inputHistogram, L);

	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			inputHistogram[input.at<uchar>(y, x)]++;
		}
	}

	return inputHistogram;
}

double* calculatePdf(double* histogram, int totalPixelSize)
{
	double* pdf = new double[L];
	fillZero(pdf, L);

	for (auto i = 0; i < L; ++i)
	{
		pdf[i] = histogram[i] / totalPixelSize;
	}

	return pdf;
}

double* calculateCdf(double* pdf)
{
	double* cdf = new double[L];
	fillZero(cdf, L);
	cdf[0] = pdf[0];

	for (auto i = 1; i < L; ++i)
	{
		cdf[i] = cdf[i - 1] + pdf[i];
	}

	return cdf;
}

Mat equalizeHistogram(Mat input)
{

	//Firstly calculate histogram
	double* inputHistogram = calculateHistogram(input);
	//Secondly extract probability density function(PDF) from the histogram
	double* inputPdf = calculatePdf(inputHistogram, input.rows * input.cols);
	//Thirdly cumulative probability function(CDF) from PDF
	double* inputCdf = calculateCdf(inputPdf);

	Mat output(input.rows, input.cols, CV_8U);

	//As last, map old image intensity values
	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			auto oldIntensity = input.at<uchar>(y, x);

			//Full implementation of Equation 3.3-8
			output.at<uchar>(y, x) = std::round((L - 1) * inputCdf[oldIntensity]);
		}
	}

	//Calculate output histogram and pdf
	double* outputHistogram = calculateHistogram(output);
	double* outputPdf = calculatePdf(outputHistogram, output.rows * output.cols);

	imshow("input histogram", dip::drawHistogram(inputPdf, L));
	imshow("output histogram", dip::drawHistogram(outputPdf, L));

	return output;
}