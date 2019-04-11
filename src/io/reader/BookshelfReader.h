/* Copyright 2014-2018 Rsyn
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

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BookshelfReader.h
 * Author: jucemar
 *
 * Created on 20 de Fevereiro de 2017, 19:03
 */

#ifndef RSYN_BOOKSHELFREADER_H
#define RSYN_BOOKSHELFREADER_H

#include "session/Session.h"
#include "core/Rsyn.h"

namespace Rsyn {

class BookshelfReader : public Reader {
       protected:
        Rsyn::Session session;
        Rsyn::Design clsDesign;
        Rsyn::Module clsModule;

        std::string optionBenchmark;
        std::string path;
        double optionTargetUtilization;

       public:
        BookshelfReader() {}
        BookshelfReader(const BookshelfReader& orig) {}
        virtual ~BookshelfReader() {}
        virtual bool load(const Rsyn::Json& params) override;
};  // end class

}  // end namespace

#endif /* RSYN_BOOKSHELFREADER_H */
