#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;
using namespace std;

void unsharpMask(cv::Mat& im);
void erosion(cv::Mat& im, int iterations, int elem, int size);
void dilation(cv::Mat& im, int iterations, int elem, int size);
void question1();
void question2();

Rect getRoi(cv::Mat &src);
void roiCallBack(int event, int x, int y, int flags, void* ptr);

