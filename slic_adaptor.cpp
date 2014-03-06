#include "slic_adaptor.h"

#include "slic.h"

cv::Mat convertBGRtoARGB(const cv::Mat& input)
{
	cv::Mat result = cv::Mat::zeros(input.size(), CV_8UC4);
	//cv::cvtColor(input, input, CV_BGR2RGB);
	const cv::Vec3b *input_ptr = input.ptr<cv::Vec3b>(0);
	cv::Vec4b *convinput_ptr = result.ptr<cv::Vec4b>(0);
	//convert from BGR to ARGB
	for(std::size_t i = 0; i < input.total(); ++i)
	{
		(*convinput_ptr)[0] = 0;
		(*convinput_ptr)[3] = (*input_ptr)[0];
		(*convinput_ptr)[2] = (*input_ptr)[1];
		(*convinput_ptr)[1] = (*input_ptr)[2];
		++input_ptr;
		++convinput_ptr;
	}

	return result;
}

cv::Mat convertARGBtoBGR(const cv::Mat& input)
{
	cv::Mat result = cv::Mat(input.size(), CV_8UC3);;

	const cv::Vec4b *outputconv_ptr = input.ptr<cv::Vec4b>(0);
	cv::Vec3b *output_ptr = result.ptr<cv::Vec3b>(0);
	//convert from ARGB to BGR
	for(std::size_t i = 0; i < input.total(); ++i)
	{
		(*output_ptr)[0] = (*outputconv_ptr)[3];
		(*output_ptr)[1] = (*outputconv_ptr)[2];
		(*output_ptr)[2] = (*outputconv_ptr)[1];
		++outputconv_ptr;
		++output_ptr;
	}

	return result;
}

void drawLabelContours(cv::Mat& image, cv::Mat& labels_cv)
{
	int* labels = labels_cv.ptr<int>(0);

	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	int width = image.cols;
	int height = image.rows;

	int sz = width*height;
	std::vector<bool> istaken(sz, false);
	std::vector<int> contourx(sz);
	std::vector<int> contoury(sz);
	int mainindex(0);int cind(0);
	for( int j = 0; j < height; j++ )
	{
		for( int k = 0; k < width; k++ )
		{
			int np(0);
			for( int i = 0; i < 8; i++ )
			{
				int x = k + dx8[i];
				int y = j + dy8[i];

				if( (x >= 0 && x < width) && (y >= 0 && y < height) )
				{
					int index = y*width + x;

					//if( false == istaken[index] )//comment this to obtain internal contours
					{
						if( labels[mainindex] != labels[index] ) np++;
					}
				}
			}
			if( np > 1 )
			{
				contourx[cind] = k;
				contoury[cind] = j;
				istaken[mainindex] = true;
				//img[mainindex] = color;
				cind++;
			}
			mainindex++;
		}
	}

	int numboundpix = cind;//int(contourx.size());
	for( int j = 0; j < numboundpix; j++ )
	{
		image.at<cv::Vec3b>(contoury[j], contourx[j]) = cv::Vec3b(255,255,255);

		for( int n = 0; n < 8; n++ )
		{
			int x = contourx[j] + dx8[n];
			int y = contoury[j] + dy8[n];
			if( (x >= 0 && x < width) && (y >= 0 && y < height) )
			{
				if(!istaken[y*width + x]) image.at<cv::Vec3b>(y, x) = cv::Vec3b(0,0,0);
			}
		}
	}
}

int slicSuperpixels(cv::Mat bgr_input, cv::Mat& labels, int superpixelsize, double compactness)
{
	assert(bgr_input.channels() == 3);
	cv::Mat input = convertBGRtoARGB(bgr_input);

	int* labels_ptr = new int[input.total()];
	int numlabels(0);
	unsigned int* img = input.ptr<unsigned int>(0);
	oldSLIC slic;
	slic.DoSuperpixelSegmentation_ForGivenSuperpixelSize(img,input.cols, input.rows, labels_ptr,numlabels,superpixelsize,compactness);

	labels = cv::Mat(input.size(), CV_32SC1);
	memcpy(labels.data, labels_ptr, input.total()*sizeof(int));
	delete[] labels_ptr;

	return numlabels;
}

