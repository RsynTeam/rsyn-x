/* Copyright 2014-2017 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef REDIRECTOR_H
#define REDIRECTOR_H

#include <streambuf>
	using std::streambuf;
#include <iostream>
	using std::ostream;

#include <wx/wx.h>

class Redirector : public streambuf {
       private:
               streambuf *clsOriginalStreamBuffer;
               ostream * clsOutStream;
               wxTextCtrl *clsTextBox;

       public:

               Redirector( wxTextCtrl *textbox, ostream &out ) {
                       clsOutStream = &out;
                       clsOriginalStreamBuffer = out.rdbuf();
                       clsTextBox = textbox;

                       clsOutStream->rdbuf( this );
               } // end constructor

               ~Redirector() {
                       clsOutStream->rdbuf( clsOriginalStreamBuffer );
               } // end destructor

               int overflow( int ch ) {
                       clsTextBox->WriteText( wxChar( ch ) );
                       return ch;
               } // end overloaed method
}; // end class

#endif