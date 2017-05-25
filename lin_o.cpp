#include <mav_trajectory_generation/polynomial_optimization_linear.h>

//Create a list of three (x,y,z) vertices to fly through, e.g. (0,0,1) -> (1,2,3) -> (2,1,5), and define some parameters. The dimension variable denotes the spatial dimension of the path (here, 3D). The derivative_to_optimize should usually be set to the last derivative that should be continuous (here, snap).


mav_trajectory_generation::Vertex::Vector vertices;
const int dimension = 3;
const int derivative_to_optimize = mav_planning_utils::derivative_order::SNAP;
//mav_trajectory_generation::Vertex start(dimension), middle(3dimension end(dimension);
mav_trajectory_generation::Vertex start(3), middle(3), end(3);

//Add constraints to the vertices.


start.makeStartOrEnd(Eigen::Vector3d(0,0,1), derivative_to_optimize);
vertices.push_back(start);

middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION, Eigen::Vector3d(1,2,3));
vertices.push_back(middle);

end.makeStartOrEnd(Eigen::Vector3d(2,1,5), derivative_to_optimize);
vertices.push_back(end);


//Compute the segment times.

std::vector<double> segment_times;
const double v_max = 2.0;
const double a_max = 2.0;
const double magic_fabian_constant = 6.5; // A tuning parameter.
segment_times = estimateSegmentTimes(vertices, v_max, a_max, magic_fabian_constant);


//Create an optimizer object and solve. The template parameter (N) denotes the number of coefficients of the underlying polynomial, which has to be even. If we want the trajectories to be snap-continuous, N needs to be at least 10.

const int N = 10;
mav_trajectory_generation::PolynomialOptimization<N> opt(dimension);
opt.setupFromVertices(vertices, segment_times, derivative_to_optimize);
opt.solveLinear();

//Obtain the polynomial segments.

mav_trajectory_generation::Segment::Vector segments;
opt.getSegments(&segments);


