/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#ifndef __BAR_BOOLEANOPERATOR_H__
#define	__BAR_BOOLEANOPERATOR_H__

#include "BooleanOperator.h"
#include "Nuga/include/Edge.h"

namespace DELAUNAY
{
class MeshData;
}
namespace NUGA
{
  
class BAR_BooleanOperator : public BooleanOperator
{
public:
  typedef BooleanOperator parent_type;
public:
  /// Constructor with the 2 input BARs B1 & B2.
  BAR_BooleanOperator(const K_FLD::FloatArray& coord1, const K_FLD::IntArray& cB1,
                      const K_FLD::FloatArray& coord2, const K_FLD::IntArray& cB2,
                      E_Float tolerance);
  /// Conformized union of the input surfaces.
  E_Int getSum(K_FLD::FloatArray& pos, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  ///B1 -  Intersection(B1, B2).
  E_Int get_1_minus_2(K_FLD::FloatArray& pos, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  ///B2 -  Intersection(B1, B2).
  E_Int get_2_minus_1(K_FLD::FloatArray& pos, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  ///  Sum(B1, B2) - Intersection(B1, B2).
  E_Int getUnion(K_FLD::FloatArray& pos, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  /// Intersection between B1 & B2.
  E_Int getIntersection(K_FLD::FloatArray& pos, K_FLD::IntArray& connect, std::vector<E_Int>& colors);
  /// Intersection border (BARs for TRI, POINTs for BAR) 
  E_Int getIntersectionBorder(K_FLD::FloatArray& coord, K_FLD::IntArray& connect);
  
  /// Destructor.
  ~BAR_BooleanOperator(void);
  
private:
  ///
  BAR_BooleanOperator(void);
  BAR_BooleanOperator(const BAR_BooleanOperator& orig);
  ///checks if the input is coplanar et reorient if required
  E_Int check_sanity();
  ///
  E_Int compute_zones();
  ///
  void convert_to_hset(const K_FLD::IntArray& cB, std::set<K_MESH::Edge>& hB);
  
  
private:
  ///
  DELAUNAY::MeshData* _dT3;
  ///
  K_FLD::IntArray     _connectInter;
  ///
  K_FLD::IntArray     _connectUnion;
  ///
  E_Float _normal[3];

};

}

#endif	/* __BAR_BOOLEANOPERATOR_H__ */

