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
