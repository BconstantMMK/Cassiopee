/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#ifndef __DELAUNAY_MESH_UTILS_1D_H__
#define __DELAUNAY_MESH_UTILS_1D_H__

#include "Nuga/include/defs.h"
#include "Nuga/include/DynArray.h"
#include "Nuga/include/maths.hxx"
#include <vector>

namespace DELAUNAY
{


class MeshUtils1D
{
public:
  MeshUtils1D(void);
  ~MeshUtils1D(void);

  template <E_Int DIM>
  static void mesh_line (const E_Float* P0, const E_Float* P1, E_Int N,
    K_FLD::FloatArray& pos, K_FLD::IntArray& connect);

  template <E_Int DIM>
  static void mesh_arc (E_Float* C, E_Float* axis, E_Float* P0, E_Float alpha, E_Int N,
    K_FLD::FloatArray& pos, K_FLD::IntArray& connect);

  static void
    compute_iso_metric(const K_FLD::FloatArray& pos, const K_FLD::IntArray& connect,
    const std::vector<E_Int>& hard_nodes,
    std::vector<E_Float> & metric, E_Float &hmax, E_Float& hmin);
};

template <E_Int DIM>
void
MeshUtils1D::mesh_line
(const E_Float* P0, const E_Float* P1, E_Int N,
 K_FLD::FloatArray& pos, K_FLD::IntArray& connect)
{
  E_Float E[DIM];
  E_Float Pi[DIM];
  E_Int   Ei[2];

  NUGA::diff<DIM>(P1, P0, E);
  E_Float L = NUGA::normalize<DIM>(E);
  E_Float ti = L/N;

  pos.pushBack(P0, P0+DIM);
  E_Int I = pos.cols()-1;

  for (E_Int i = 0; i < N; ++i)
  {
    for (E_Int j = 0; j < DIM; ++j)
      Pi[j] = pos(j, I) + ti * E[j];

    pos.pushBack(Pi, Pi+DIM);
    Ei[0] = ++I-1;
    Ei[1] = I;
    connect.pushBack(Ei, Ei+2);
  }
}

template <E_Int DIM>
void
MeshUtils1D::mesh_arc (E_Float* C, E_Float* axis, E_Float* P0, E_Float alpha, E_Int N,
                       K_FLD::FloatArray& pos, K_FLD::IntArray& connect)
{
  E_Float R = ::sqrt(NUGA::sqrDistance(C, P0, DIM));
  E_Float ai = alpha*R/N;
  E_Float Pj[DIM], CPi[DIM], PiPj[DIM];
  const E_Float* Pi;
  E_Int Ei[2], I;

  pos.pushBack(P0, P0+DIM);
  I = pos.cols()-1;

  NUGA::normalize<3>(axis);

  for (E_Int i = 0; i < N; ++i)
  {
    Pi = pos.col(I);
    NUGA::diff<DIM>(Pi, C, CPi);
    NUGA::crossProduct<3>(axis, CPi, PiPj);
    NUGA::normalize<3>(PiPj);

    for (E_Int j = 0; j < DIM; ++j)
      Pj[j] = *(Pi+j) + ai * PiPj[j];

    pos.pushBack(Pj, Pj+DIM);

    Ei[0] = ++I-1;
    Ei[1] = I;

    connect.pushBack(Ei, Ei+2);
  }
}

}


#endif

