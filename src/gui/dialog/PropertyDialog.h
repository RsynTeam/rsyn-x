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

#ifndef RSYN_QT_PROPERTY_DIALOG_H
#define RSYN_QT_PROPERTY_DIALOG_H

#include <QDialog>
#include "ui_PropertyDialog.h"

#include <iostream>

namespace Rsyn {

namespace QtUtils {
class QTreeDescriptor;
}  // end namespace

class PropertyDialog : public QDialog, public Ui::PropertyDialog {
        Q_OBJECT
       public:
        PropertyDialog(QWidget *parent = 0);
        void setProperties(const QtUtils::QTreeDescriptor &properties);

       private:
};  // end class

}  // end namespace

#endif
