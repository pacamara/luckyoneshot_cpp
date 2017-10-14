/*
# Copyright (C) 2017 Cryobrew Software (http://www.cryobrew.com)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
*/

#include <sys/time.h>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>
#include <zbar.h>
#include "dangampola_grad_method.h"
#include "QRScanner.h"

using namespace cv;
using namespace std;

const long getSystemMillis() {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	const long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	return ms;
}

int main(int argc, char* argv[]) {
	const long mainStart = getSystemMillis();

	const char* INPUT_FILE = argv[1];
	printf("INPUT_FILE=%s \n", INPUT_FILE);

	const bool IS_DEV_BOX = (3==argc) ? (::strcmp(argv[2], "True") == 0) : false;
	printf("IS_DEV_BOX=%d \n", IS_DEV_BOX);

    // NB. No need to downsample if only getting the rects.
    // Not downsampling improves number of QR codes recognized.

    Mat rgb = imread(INPUT_FILE);

    // Separate copy which will be decorated (for user feedback)
    Mat rgbDecorated = rgb.clone();

    // Detect edges of possible QR codes
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    detectBoundingBoxes(rgb, contours, hierarchy);

    int imageLabel = 0;

    // Use ordered set so will be in ascending numerical order
    set<string> scannedCodes;
	QRScanner scan;

    // Size of QR code in a grid of 3 rowsx4 cols=12 receipts taken
    // in portrait mode on nexus phone is about 150-170 pixels squared
	// (seems minimum for reliable recognition).
	// 80x80 is in fact totally unrecognizable but this min size ensures only
	// QR codes are processed.
	const int MIN_SIZE = 80;

	const long scanLoopStart = getSystemMillis();
    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
    	imageLabel++;
        const Rect rect = boundingRect(contours[idx]);
        const float aspectRatio = (float)rect.width / (float)rect.height;
        if (aspectRatio > 0.9 && aspectRatio < 1.1 && rect.width > MIN_SIZE) {
        	try {
				// Suspected QR code, scan it
				const long scanStart = getSystemMillis();
				const Mat rectMat = rgb(rect);
				const string sc = scan.scan((const Mat&)rectMat, imageLabel, IS_DEV_BOX);
				printf("Time to scan=%lu \n", getSystemMillis()-scanStart);

				if (sc.length()!=0) {
					assert(sc.length()==17);
					scannedCodes.insert(sc);

					const int LINETYPE = 8;
					const Point centre(rect.x+(rect.width/2), rect.y+(rect.height/2));
					circle(rgbDecorated,
							centre,
							(int)((float)rect.width/1.25),
							Scalar(0, 255, 200),
							30,
							LINETYPE);
				}

				if (IS_DEV_BOX) {
					imwrite("z" +
							std::to_string(imageLabel) + ".jpg", rectMat);
				}
        	}
        	catch (const exception& e) {
        		cout << e.what() << '\n';
    		}
        }
    }

	printf("**** Time for scan loop=%lu \n", getSystemMillis()-scanLoopStart);

	// Shrink/degrade user feedback image to avoid using too much bandwidth,
	// and overwrite original image file with it.
    const float scale_img = 320.0 / (float)rgb.cols;
	resize(rgbDecorated,rgbDecorated,
		 Size(rgbDecorated.cols*scale_img, rgbDecorated.rows*scale_img));
	const int JPG_QUALITY = 30;
	vector<int> params;
	params.push_back(CV_IMWRITE_JPEG_QUALITY);
	params.push_back(JPG_QUALITY);
	imwrite(INPUT_FILE, rgbDecorated, (const vector<int>&)params);

    /* Can be useful when debugging
        if (IS_DEV_BOX) {
    	imshow("location of recognized QR codes",rgbDecorated);
    	waitKey(0);
    } */

	int i = 0;
	string scannedCodesStr = "";
	for (auto sc:scannedCodes) {
		if (i>0) {
			scannedCodesStr += ",";
		}
		scannedCodesStr += sc;
		i++;
	}
	printf("scannedCodes=%s\n", scannedCodesStr.c_str());

	printf("**** Time for all of main()=%lu \n", getSystemMillis()-mainStart);
    return 0;
}
