/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#ifndef __DELAUNAY_PREDICATES_H__
#define __DELAUNAY_PREDICATES_H__

#include "Nuga/include/Triangle.h"
#include "Metric.h"

namespace DELAUNAY
{

/**
   Predicate that returns true if the input edge is hard.
  */
  struct HardEdgeCriterion : public std::unary_function <const K_MESH::NO_Edge&, bool>
{

  explicit HardEdgeCriterion(const NUGA::non_oriented_edge_set_type& hard_edges)
    :_hard_edges(hard_edges){}

  inline bool operator() (const K_MESH::NO_Edge& Ei) const
  {
      return (_hard_edges.find(Ei) != _hard_edges.end());
  }

  const NUGA::non_oriented_edge_set_type& _hard_edges;
};

///
template <typename T>
struct no_predicate : public std::unary_function <T, bool>
{
  inline bool operator() (const T& ) const
  {
    return false;
  }
};


/**
   Predicate that returns true if the input node is in the K ball.
   */

template <typename T>
struct DelaunayCriterion : public std::unary_function <E_Int, bool>
{
  explicit DelaunayCriterion(const K_FLD::FloatArray& pos, const K_FLD::IntArray& connect)
    :_pos(pos), _connect(connect), _m(0){}

  inline void setPoint(const E_Float* pt, const T& m){_pt = pt;_m = &m;}

  inline bool operator() (E_Int K) const
  {
    E_Float              R2, C[2];

    _t.circumDiskCenter(_pos, _connect.col(K), R2, C, *_m);

    return (R2 > 0.) ? (distance2(C, _pt) < R2 * (1.0 + 1.e-10)) : false;//fixme : tol
  }
 
  inline E_Float distance2(const E_Float* P0, const E_Float* P1) const
  {
    E_Float u = P1[0] - P0[0];
    E_Float v = P1[1] - P0[1];
    return ((*_m)[0] * u * u) + (2. * ((*_m)[1] * u * v) +  ((*_m)[2] * v * v));
  }

  const K_FLD::FloatArray& _pos;
  const K_FLD::IntArray&   _connect;
  const E_Float*           _pt;
  const T*                 _m;
  K_MESH::Triangle         _t;
};

//iso specialization
template <> inline
E_Float
DelaunayCriterion<E_Float>::distance2(const E_Float* P0, const E_Float* P1) const
{
  E_Float u = P1[0] - P0[0];
  E_Float v = P1[1] - P0[1];
  return (u * u) + (v * v); 
}

}

#endif
