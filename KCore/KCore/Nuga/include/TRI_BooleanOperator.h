/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#ifndef __BOOLEAN_OPERATOR_H__
#define __BOOLEAN_OPERATOR_H__

#include "BooleanOperator.h"
#include "Nuga/include/Triangle.h"
#include <set>
#include <map>

namespace NUGA
{

class TRI_BooleanOperator : public BooleanOperator
{
public:
  typedef BooleanOperator parent_type;

public:
  /// Constructor with the 2 input surfaces S1 & S2.
  TRI_BooleanOperator(const K_FLD::FloatArray& coord1, const K_FLD::IntArray& connect1,
                  const K_FLD::FloatArray& coord2, const K_FLD::IntArray& connect2,
                  E_Float tolerance);
  /// Destructor.
  ~TRI_BooleanOperator(void);
  
  /// Check wether the input mesh is close or not
  static bool isClosed(const K_FLD::FloatArray& coord, const K_FLD::IntArray& connect);

public:
  /// Conformized union of the input surfaces.
  E_Int getSum(K_FLD::FloatArray& coord, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  ///S1 -  Intersection(S1, S2).
  E_Int get_1_minus_2(K_FLD::FloatArray& coord, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  ///S2 -  Intersection(S1, S2).
  E_Int get_2_minus_1(K_FLD::FloatArray& coord, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  ///  Sum(S1, S2) - Intersection(S1, S2).
  E_Int getUnion(K_FLD::FloatArray& coord, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  /// Intersection between S1 & S2.
  E_Int getIntersection(K_FLD::FloatArray& coord, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  /// Intersection border (BAR)
  E_Int getIntersectionBorder(K_FLD::FloatArray& coord, K_FLD::IntArray& connect);

  static int case_nb;

private:
  ///
  E_Int compute_zones();
  ///
  E_Int check_sanity();
  ///
  E_Int __compute_x_contour(std::set<K_MESH::NO_Edge>& hXC);
  ///
  E_Int __get_mesh_edges(const K_FLD::IntArray& connect, std::vector<K_MESH::NO_Edge>& no_edges);
  ///
  E_Int __find_external_element(const K_FLD::FloatArray& coord, K_FLD::IntArray* connects, E_Int& S0, E_Int& i);
  ///
  E_Int __alternate_coloring(const K_FLD::IntArray& connect, E_Int S0, const std::set<K_MESH::NO_Edge>& hXC, std::vector<E_Int>& colors);
  ///
  void __getZoneNeighbouring(const K_FLD::IntArray& connect, std::vector<E_Int>& zones, std::map<E_Int, std::set<E_Int> >& zone_to_zones);
  ///
  E_Int __reorient_x_contour(const K_FLD::IntArray& connect, const std::set<K_MESH::NO_Edge>& hXC, std::set<K_MESH::Edge>& hXCO);
  ///
  E_Int __color_other_part(K_FLD::IntArray& connect, const std::set<K_MESH::NO_Edge>& hXC, const std::set<K_MESH::Edge>& hXCO, std::vector<E_Int>& colors);
  ///
  E_Int __getCommonTriangles(K_FLD::IntArray& connectC);
  ///
  void __fillZoneContainers(const std::vector<E_Int> & colors, const K_FLD::IntArray& connect,
                            K_FLD::IntArray* pCin, K_FLD::IntArray* pCout, K_FLD::IntArray* pCinter);
  ///
  void __splitOverlapByOrientation(const K_FLD::IntArray& connectInter, K_FLD::IntArray& connectIS, K_FLD::IntArray& connectIO);

private:
  ///
  K_FLD::IntArray     _connect_1_in_2;
  ///
  K_FLD::IntArray     _connect_2_in_1;
  ///
  K_FLD::IntArray     _connect_inter_same;
  ///
  K_FLD::IntArray     _connect_inter_opcoordite_1;
  
  ///
  std::set<K_MESH::Edge> _hXCO;
  

};

}

#endif

