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

#ifndef _QRSCANNER_H_
#define _QRSCANNER_H_

#include <iostream> // Needed by zbar header files
#include <zbar.h>

#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/utility.hpp"

using namespace cv;
using namespace std;
using namespace zbar;

class QRScanner
{
    public:
        QRScanner();
        string scan(const Mat& imageIn, const int imageLabel, const bool IS_DEV_BOX);

    private:
        const string EMPTY_STRING;
        ImageScanner scanner;
        string getInvoiceCode(const string& symbolType, const string& symbolData);
};

#endif

