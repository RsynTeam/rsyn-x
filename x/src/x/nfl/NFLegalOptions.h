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
 * File:   NFLegalOptions.h
 * Author: jucemar
 *
 * Created on July 14, 2018, 7:57 AM
 */

#ifndef NFLEGALOPTIONS_H
#define NFLEGALOPTIONS_H

#include <string>

namespace NFL {

enum class CellOverlapOpto {
	NONE = 0,
	NFLEGAL = 1,
	JEZZ = 2,
	NUM_CELL_OVERLAP_OPTO = 3
}; // end enum 

class NFLegalOptions {
protected:
	CellOverlapOpto clsCellOverlapOpto = CellOverlapOpto::NONE;
	
public:
	NFLegalOptions() = default;
	~NFLegalOptions() = default;
	
	void setCellOverlapOpto(const CellOverlapOpto opto) {
		clsCellOverlapOpto = opto;
	} // end method 
	
	
	CellOverlapOpto getCellOverlapOpto() const {
		return clsCellOverlapOpto;
	} // end method 
	
	std::string getCellOverlapOptoString() const {
		if(clsCellOverlapOpto == CellOverlapOpto::NFLEGAL) {
			return "NFLEGAL";
		} else if (clsCellOverlapOpto == CellOverlapOpto::JEZZ) {
			return "JEZZ";
		} else {
			return "NONE";
		} // end if-else 
	} // end method 
	
}; // end class 

} // end namespace 

#endif /* NFLEGALOPTIONS_H */

