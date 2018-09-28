/**
 * \file matrix_geometry.hh
 * \brief Definition of simple typdef to ease the code writting and reading
 * \author Maximilien Naveau
 * \date 2018
 *
 * This file declares simple typdef to ease the code writting and reading.
 * usage: see demos and unit tests and documentation
 */

#ifndef MATRIX_GEOMETRY_HH
#define MATRIX_GEOMETRY_HH

#include <Eigen/Geometry>
#include <Eigen/Core>
#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/eigen-io.h>

#  define MRAWDATA(x) x.data()

namespace dynamic_graph {
  typedef Eigen::Transform<double,3, Eigen::Affine>  MatrixHomogeneous;
  typedef Eigen::Matrix<double,3,3>  MatrixRotation;
  typedef Eigen::AngleAxis<double>  VectorUTheta;
  typedef Eigen::Quaternion<double> VectorQuaternion;
  typedef Eigen::Vector3d VectorRotation;
  typedef Eigen::Vector3d VectorRollPitchYaw;
  typedef Eigen::Matrix<double,6,6> MatrixForce;
  typedef Eigen::Matrix<double,6,6> MatrixTwist;

  inline void buildFrom (const MatrixHomogeneous& MH, MatrixTwist& MT) {

    Eigen::Vector3d _t = MH.translation();
    MatrixRotation R(MH.linear());
    Eigen::Matrix3d Tx;
    Tx << 0, -_t(2), _t(1),
        _t(2), 0, -_t(0),
        -_t(1), _t(0), 0;
    Eigen::Matrix3d sk; sk = Tx*R;

    MT.block<3,3>(0,0) = R;
    MT.block<3,3>(0,3) = sk;
    MT.block<3,3>(3,0).setZero();
    MT.block<3,3>(3,3) = R;
  }
} // namespace dynamic_graph

#endif // MATRIX_GEOMETRY_HH