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

#ifndef RSYN_PHYSICAL_TRANSFORMATION_H
#define RSYN_PHYSICAL_TRANSFORMATION_H

#include "rsyn/phy/util/PhysicalTypes.h"
#include "rsyn/util/dbu.h"

namespace Rsyn {

class PhysicalTransform {
public:

	PhysicalTransform() : referencePoint(0, 0), orientation(ORIENTATION_N) {}
	PhysicalTransform(const DBUxy referencePoint, const PhysicalOrientation orientation) : referencePoint(referencePoint), orientation(orientation) {}

	DBUxy transform(const DBUxy &p) const {
		// Translate to the reference point.
		const DBUxy v = p - referencePoint;

		// Apply transformation.
		const DBU (&M)[2][2] = orientation == ORIENTATION_INVALID?
			TRANSFORMATION_MATRIXES[ORIENTATION_N] :
			TRANSFORMATION_MATRIXES[orientation];
		const DBU tx = (v.x*M[0][0]) + (v.y*M[0][1]);
		const DBU ty = (v.x*M[1][0]) + (v.y*M[1][1]);

		// Translate back to the original position.
		const DBUxy q = DBUxy(tx, ty) + referencePoint;

		// Return.
		return q;
	} // end method

	DBUxy getReferencePoint() const { return referencePoint; }
	PhysicalOrientation getOrientation() const { return orientation; }

	void setReferencePoint(const DBUxy refPoint) {referencePoint = refPoint;}
	void setOrientation(const PhysicalOrientation &orient) {orientation = orient;}

private:

	DBUxy referencePoint;
	PhysicalOrientation orientation;

	static const DBU TRANSFORMATION_MATRIXES[NUM_PHY_ORIENTATION][2][2];
}; // end class

} // end namespace

#endif /* TRANSFORMATION_H */

