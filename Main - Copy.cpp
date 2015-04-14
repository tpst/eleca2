#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;
using namespace std;

void unsharpMask(cv::Mat& im);
void erosion(cv::Mat& im, int iterations, int elem, int size);
void dilation(cv::Mat& im, int iterations, int elem, int size);

int main()
{

	bool unsharp = true;

	Mat src = imread("tajmahal2004.jpg");
	Mat dst, canny_dst;
	Mat dif;

	Mat temp = src.clone();
	int canny_low = 51;
	int canny_low_max = 500;
	int canny_high = 165;
	int canny_high_max = 500;
	int hough_thresh = 1;
	int hough_thresh_max = 500;
	int min_line_length = 30;
	int max_line_gap = 10;

	int d = 15;
	int sigmaColor = 80;
	int sigmaSpace = 80;

	namedWindow("dst", CV_WINDOW_NORMAL);
	createTrackbar("d", "dst", &d, 50);
	createTrackbar("sc", "dst", &sigmaColor, 150);
	createTrackbar("ss", "dst", &sigmaSpace, 150);
	//sharpen image
	unsharpMask(src);

		while (1)
			{
				bilateralFilter(src, dst, d, sigmaColor, sigmaSpace);
				
				cv::addWeighted(dst, 1.5, src, -0.5, 0, dst);
				//dilation(dst, 3, 0, 3);
				imshow("dst", dst);
				if (waitKey(0) == 32)
				{
					continue;
				}
				else {
					break;
				}
			}

	//dst = src;

	/////////////////////////////
	// do edge detection

	namedWindow("Canny", CV_WINDOW_NORMAL);
	createTrackbar("canny low", "Canny", &canny_low, canny_low_max);
	createTrackbar("canny high", "Canny", &canny_high, canny_high_max);
	cvtColor(dst, dst, CV_BGR2GRAY);
	while (waitKey(1) != 32)
	{
		Canny(dst, canny_dst, canny_low, canny_high, 3);
		dilation(canny_dst, 3, 0, 3);
		imshow("Canny", canny_dst);
	}
	destroyAllWindows();

	namedWindow("lines", CV_WINDOW_NORMAL);
	namedWindow("tbar", CV_WINDOW_NORMAL);
	createTrackbar("hough thresh", "tbar", &hough_thresh, hough_thresh_max);
	createTrackbar("max line length", "tbar", &min_line_length, 100);
	createTrackbar("max line gap", "tbar", &max_line_gap, 50);

	vector<Vec2f> lines;
	HoughLines(canny_dst, lines, 1, CV_PI / 180, 100);

	std::vector<cv::Vec2f>::const_iterator it = lines.begin();
	while (it != lines.end()) {
		float rho = (*it)[0]; // first element is distance rho
		float theta = (*it)[1]; // second element is angle theta
		if (theta < CV_PI / 4.
			|| theta > 3.*CV_PI / 4.) { // ~vertical line
			// point of intersection of the line with first row
			cv::Point pt1(rho / cos(theta), 0);
			// point of intersection of the line with last row
			cv::Point pt2((rho - dst.rows*sin(theta)) /
				cos(theta), dst.rows);
			// draw a white line
			cv::line(temp, pt1, pt2, cv::Scalar(255), 1);
		}
		else { // ~horizontal line
			// point of intersection of the
			// line with first column
			cv::Point pt1(0, rho / sin(theta));
			// point of intersection of the line with last column
			cv::Point pt2(dst.cols,
				(rho - dst.cols*cos(theta)) / sin(theta));
			// draw a white line
			cv::line(temp, pt1, pt2, cv::Scalar(255), 1);
		}
		++it;
	}
	imshow("lines", temp);
	waitKey(0);
	//while (1)
	//{
	//	Mat temp = src.clone();

	//	vector<Vec4i> lines;
	//	HoughLinesP(canny_dst, lines, 1, CV_PI / 180, hough_thresh, min_line_length, max_line_gap);
	//	for (size_t i = 0; i < lines.size(); i++)
	//	{
	//		line(temp, Point(lines[i][0], lines[i][1]),
	//			Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
	//	}

	//	imshow("lines", temp);
	//	waitKey(1);
	//}
	
	return 0;
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