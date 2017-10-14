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

#include "QRScanner.h"

QRScanner::QRScanner() {
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
}

bool isNumber(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

/** Returns the shop code+lucky number+date in the QR code if exists, else EMPTY_STRING.
 * 	First 17 digits are shop(2) + lucky number(8) + KMT date(7) */
string QRScanner::getInvoiceCode(const string& symbolType, const string& symbolData) {
    if (symbolType.compare("QR-Code")==0 && symbolData.length() >= 17) {
    	const char c0 = symbolData.at(0);
    	const char c1 = symbolData.at(1);
    	if (c0>='A' && c0<='Z' && c1>='A' && c1<='Z') {
       		const string ln = symbolData.substr(2, 17-2);
       		assert(ln.length()==15);
			if (isNumber(ln)) {
	       		const string all = symbolData.substr(0, 17);
				printf("Found lucky number+date=%s \n",all.c_str());
				return all;
			}
    	}
    }

    return EMPTY_STRING;
}

string QRScanner::scan(const Mat& imageIn, const int imageLabel, const bool IS_DEV_BOX) {
	Mat imageOut;
	cvtColor(imageIn,imageOut,CV_BGR2GRAY);

	const int width = imageOut.cols;
	const int height = imageOut.rows;
	uchar *raw = (uchar *)imageOut.data;
	Image image(width, height, "Y800", raw, width * height);
    scanner.scan(image);

    // Process any codes that were found
    for(Image::SymbolIterator symbol = image.symbol_begin();
            symbol != image.symbol_end();
            ++symbol) {
        if (IS_DEV_BOX) {
			cout << "!!!!!!!!!!!!!! decoded typeName=" << symbol->get_type_name()
				 << " symbol=" << symbol->get_data() << endl;
			printf("Symbol length=%d \n", (int)symbol->get_data().length());
        }

		return getInvoiceCode(symbol->get_type_name(),symbol->get_data());
    }

    image.set_data(NULL, 0);
    return EMPTY_STRING;
}

