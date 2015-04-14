#include "q1.h"

void question1()
{
	int thresh = 125;
	int canny_low = 51;
	int canny_low_max = 500;
	int canny_high = 165;
	int canny_high_max = 500;
	int hough_thresh = 1;
	int hough_thresh_max = 500;
	int min_line_length = 100;
	int max_line_gap = 14;
	Mat src = imread("tajmahal2004.jpg");
	Mat dst, canny_dst, dst1;

	int min_angle = 170;
	int max_angle = 148;

	//Apply median filter
	medianBlur(src, dst, 5);
	imshow("source", src);
	imshow("result", dst);
	waitKey(0);

	unsharpMask(dst);
	imshow("dst", dst);
	waitKey(0);

	cvtColor(dst, dst, CV_BGR2GRAY);
	namedWindow("thresh");
	createTrackbar("thresh", "thresh", &thresh, 255);
	while (waitKey(1) != 32)
	{
		threshold(dst, dst1, thresh, 255, 0);
		//erosion(dst1, 10, 0, 1);
		//dilation(dst1, 10, 0, 1);

		imshow("thresh", dst1);

	}

	namedWindow("Canny", CV_WINDOW_NORMAL);
	createTrackbar("canny low", "Canny", &canny_low, canny_low_max);
	createTrackbar("canny high", "Canny", &canny_high, canny_high_max);
	while (waitKey(1) != 32)
	{
		Canny(dst, canny_dst, canny_low, canny_high, 3);
		erosion(dst1, 10, 0, 1);
		dilation(dst1, 10, 0, 1);
		imshow("Canny", canny_dst);
	}


	namedWindow("lines", CV_WINDOW_NORMAL);
	namedWindow("tbar", CV_WINDOW_NORMAL);
	createTrackbar("hough thresh", "tbar", &hough_thresh, hough_thresh_max);
	createTrackbar("max line length", "tbar", &min_line_length, 100);
	createTrackbar("max line gap", "tbar", &max_line_gap, 50);
	createTrackbar("min angle", "tbar", &min_angle, 360);
	createTrackbar("max angle", "tbar", &max_angle, 360);

	while (1)
	{
		Mat temp = src.clone();

		vector<Vec4i> lines;
		HoughLinesP(canny_dst, lines, 1, CV_PI / 180, hough_thresh, min_line_length, max_line_gap);
		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			// draw the lines

			Point p1, p2;
			p1 = Point(l[0], l[1]);
			p2 = Point(l[2], l[3]);
			//calculate angle in radian,  if you need it in degrees just do angle * 180 / PI
			float angle = atan2(p1.y - p2.y, p1.x - p2.x);
			angle = angle * (180 / CV_PI);
			if (angle > -min_angle && angle < -max_angle)
			{
				line(temp, p1, p2, Scalar(0, 0, 255), 3, 8);
			}
		}

		imshow("lines", temp);
		waitKey(1);
	}
}

void question2()
{
	vector<Mat> images;

	images.push_back(imread("MRI1_01.png"));
	images.push_back(imread("MRI1_02.png"));
	images.push_back(imread("MRI1_03.png"));
	images.push_back(imread("MRI1_04.png"));
	images.push_back(imread("MRI1_05.png"));
	images.push_back(imread("MRI1_06.png"));
	images.push_back(imread("MRI1_07.png"));
	images.push_back(imread("MRI1_08.png"));
	images.push_back(imread("MRI1_09.png"));
	images.push_back(imread("MRI1_10.png"));
	images.push_back(imread("MRI1_11.png"));
	images.push_back(imread("MRI1_12.png"));
	images.push_back(imread("MRI1_13.png"));
	images.push_back(imread("MRI1_14.png"));
	images.push_back(imread("MRI1_15.png"));
	images.push_back(imread("MRI1_16.png"));
	
	Rect roi = getRoi(images[0]);
	
	int thresh = 37;

	namedWindow("Heart", CV_WINDOW_NORMAL);

	for (int i = 0; i < 16; i++)
	{
		Mat src = images[i](roi);
		Mat frame = images[i];
		Mat dst;

		Mat mask(images[i].size(), images[i].type());
		mask.setTo(0);
		
		src.copyTo(mask(roi));

		cvtColor(mask, dst, CV_BGR2GRAY);
		GaussianBlur(dst, dst, Size(3, 3), 0, 0);
		threshold(dst, dst, thresh, 255, 0 );
		erosion(dst, 1, 2, 5);
		dilation(dst, 1, 2, 5);
		imshow("Heart", dst(roi));

		vector<vector<Point>> contours;
		findContours(dst, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		
		int largest_area = 0;
		int largest_contour_index = 0;
		//-- find the largest contour which is the heart
		for (int i = 0; i< contours.size(); i++) // iterate through each contour. 
		{
			double a = contourArea(contours[i], false);  //  Find the area of contour
			if (a>largest_area){
				largest_area = a;
				largest_contour_index = i;                //Store the index of largest contour
			}
		}
		//-- find convex hull
		vector<vector<Point>> hull(1);
		convexHull(Mat(contours[largest_contour_index]), hull[0], false);

		drawContours(frame, hull, 0, Scalar(0, 0, 255), 1, 8);
		imshow("src", frame);
		waitKey(0);

	}
}
// Perform in-place unsharp masking operation
void unsharpMask(cv::Mat& im)
{
	cv::Mat tmp;
	cv::GaussianBlur(im, tmp, cv::Size(5, 5), 5);
	cv::addWeighted(im, 1.5, tmp, -0.5, 0, im);
}

void erosion(cv::Mat& im, int iterations, int elem, int size)
{
	int erosion_type;
	if (elem == 0){ erosion_type = MORPH_RECT; }
	else if (elem == 1){ erosion_type = MORPH_CROSS; }
	else if (elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type, Size(size, size), Point(-1, -1));

	erode(im, im, element, Point(-1, -1), iterations, 1);
}

void dilation(cv::Mat& im, int iterations, int elem, int size)
{
	int erosion_type;
	if (elem == 0){ erosion_type = MORPH_RECT; }
	else if (elem == 1){ erosion_type = MORPH_CROSS; }
	else if (elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type, Size(size, size), Point(-1, -1));

	dilate(im, im, element, Point(-1, -1), iterations, 1);
}