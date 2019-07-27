// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ofMain.h"
#include "tinysplinecpp.h"

#ifndef SPLINEEVAL_H
#define SPLINEEVAL_H

class SplineEval: public ofThread{
public:
	void setupControlPoints();
	void updateControlPoints();

	struct Result {
	    std::vector<tinyspline::real> point;
	};
	struct Control {
	    glm::vec3 point;
	    glm::vec3 heading;
	};

	std::vector<Result> results;
	std::vector<Control> controls;
	std::vector<tinyspline::real> ctrlp;

private:
	void threadedFunction();
	tinyspline::BSpline spline;
};

#endif
