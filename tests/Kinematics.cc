#include <UnitTest++.h>

#include <iostream>

#include "mathutils.h"
#include "Logging.h"

#include "Model.h"

using namespace std;
using namespace SpatialAlgebra;

const double TEST_PREC = 1.0e-14;

struct KinematicsFixture {
	KinematicsFixture () {
		ClearLogOutput();
		model = new Model;
		model->Init();

		/* Basically a model like this, where X are the Center of Masses
		 * and the CoM of the last (3rd) body comes out of the Y=X=0 plane.
		 *
		 *                X
		 *                *
		 *              _/
		 *            _/  (-Z)
		 *      Z    /
		 *      *---* 
		 *      |
		 *      |
		 *  Z   |
		 *  O---*
		 *      Y
		 */

		body_a = Body (1., Vector3d (1., 0., 0.), Vector3d (1., 1., 1.));
		joint_a = Joint(
				JointTypeRevolute,
				Vector3d (0., 0., 1.)
				);

		body_a_id = model->AddBody(0, Xtrans(Vector3d(0., 0., 0.)), joint_a, body_a);

		body_b = Body (1., Vector3d (0., 1., 0.), Vector3d (1., 1., 1.));
		joint_b = Joint (
				JointTypeRevolute,
				Vector3d (0., 1., 0.)
				);

		body_b_id = model->AddBody(body_a_id, Xtrans(Vector3d(1., 0., 0.)), joint_b, body_b);

		body_c = Body (1., Vector3d (0., 0., 1.), Vector3d (1., 1., 1.));
		joint_c = Joint (
				JointTypeRevolute,
				Vector3d (0., 0., 1.)
				);

		body_c_id = model->AddBody(body_b_id, Xtrans(Vector3d(0., 1., 0.)), joint_c, body_c);

		body_d = Body (1., Vector3d (1., 0., 0.), Vector3d (1., 1., 1.));
		joint_c = Joint (
				JointTypeRevolute,
				Vector3d (1., 0., 0.)
				);

		body_d_id = model->AddBody(body_c_id, Xtrans(Vector3d(0., 0., -1.)), joint_c, body_d);

		Q = std::vector<double> (4, 0.);
		QDot = std::vector<double> (4, 0.);
		QDDot = std::vector<double> (4, 0.);
		Tau = std::vector<double> (4, 0.);

		ClearLogOutput();
	}
	
	~KinematicsFixture () {
		delete model;
	}
	Model *model;

	unsigned int body_a_id, body_b_id, body_c_id, body_d_id;
	Body body_a, body_b, body_c, body_d;
	Joint joint_a, joint_b, joint_c, joint_d;

	std::vector<double> Q;
	std::vector<double> QDot;
	std::vector<double> QDDot;
	std::vector<double> Tau;
};

TEST_FIXTURE(KinematicsFixture, TestPositionNeutral) {
	// We call ForwardDynamics() as it updates the spatial transformation
	// matrices
	ForwardDynamics(*model, Q, QDot, Tau, QDDot);

	Vector3d body_position;

	CHECK_ARRAY_CLOSE (Vector3d (0., 0., 0.), model->GetBodyOrigin(body_a_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (1., 0., 0.), model->GetBodyOrigin(body_b_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (1., 1., 0.), model->GetBodyOrigin(body_c_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (1., 1., -1.), model->GetBodyOrigin(body_d_id), 3, TEST_PREC );
}

TEST_FIXTURE(KinematicsFixture, TestPositionBaseRotated90Deg) {
	// We call ForwardDynamics() as it updates the spatial transformation
	// matrices

	Q[0] = 0.5 * M_PI;
	ForwardDynamics(*model, Q, QDot, Tau, QDDot);

	Vector3d body_position;

//	cout << LogOutput.str() << endl;
	CHECK_ARRAY_CLOSE (Vector3d (0., 0., 0.), model->GetBodyOrigin(body_a_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (0., 1., 0.), model->GetBodyOrigin(body_b_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (-1., 1., 0.),model->GetBodyOrigin(body_c_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (-1., 1., -1.), model->GetBodyOrigin(body_d_id), 3, TEST_PREC );
}

TEST_FIXTURE(KinematicsFixture, TestPositionBaseRotatedNeg45Deg) {
	// We call ForwardDynamics() as it updates the spatial transformation
	// matrices

	Q[0] = -0.25 * M_PI;
	ForwardDynamics(*model, Q, QDot, Tau, QDDot);

	Vector3d body_position;

//	cout << LogOutput.str() << endl;
	CHECK_ARRAY_CLOSE (Vector3d (0., 0., 0.), model->GetBodyOrigin(body_a_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (0.707106781186547, -0.707106781186547, 0.), model->GetBodyOrigin(body_b_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (sqrt(2), 0., 0.),model->GetBodyOrigin(body_c_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (sqrt(2), 0., -1.), model->GetBodyOrigin(body_d_id), 3, TEST_PREC );
}

TEST_FIXTURE(KinematicsFixture, TestPositionBodyBRotated90Deg) {
	// We call ForwardDynamics() as it updates the spatial transformation
	// matrices
	Q[1] = 0.5 * M_PI;
	ForwardDynamics(*model, Q, QDot, Tau, QDDot);

	Vector3d body_position;

	CHECK_ARRAY_CLOSE (Vector3d (0., 0., 0.), model->GetBodyOrigin(body_a_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (1., 0., 0.), model->GetBodyOrigin(body_b_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (1., 1., 0.),model->GetBodyOrigin(body_c_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (0., 1., 0.),model->GetBodyOrigin(body_d_id), 3, TEST_PREC );
}

TEST_FIXTURE(KinematicsFixture, TestPositionBodyBRotatedNeg45Deg) {
	// We call ForwardDynamics() as it updates the spatial transformation
	// matrices
	Q[1] = -0.25 * M_PI;
	ForwardDynamics(*model, Q, QDot, Tau, QDDot);

	Vector3d body_position;

	CHECK_ARRAY_CLOSE (Vector3d (0., 0., 0.), model->GetBodyOrigin(body_a_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (1., 0., 0.), model->GetBodyOrigin(body_b_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (1., 1., 0.),model->GetBodyOrigin(body_c_id), 3, TEST_PREC );
	CHECK_ARRAY_CLOSE (Vector3d (1 + 0.707106781186547, 1., -0.707106781186547), model->GetBodyOrigin(body_d_id), 3, TEST_PREC );
}