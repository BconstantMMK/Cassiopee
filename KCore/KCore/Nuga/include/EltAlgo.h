/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#ifndef _ELTALGO_H_
#define _ELTALGO_H_

#include "Nuga/include/DynArray.h"
#include "Nuga/include/defs.h"
#include "Nuga/include/DefContainers.h"
#include <algorithm>
#include "Nuga/include/ArrayAccessor.h"
#include "Nuga/include/ngon_unit.h"

#define NON_MANIFOLD_COL -9

namespace NUGA {

template <typename ElementType>
class EltAlgo
{
public:
  typedef NUGA::size_type                    size_type;
  typedef NUGA::int_set_type                 int_set_type;
  typedef NUGA::int_vector_type              int_vector_type;
  typedef std::vector<int_vector_type>             NeighbourType;
  typedef typename ElementType::boundary_type      BoundaryType;
  typedef std::map<BoundaryType, int_vector_type > BoundToEltType;
  typedef std::map<E_Int, int_vector_type >        NodeToNodesType;

public:

  template <typename Connectivity_t>
  static E_Int getBoundToElements (const K_FLD::ArrayAccessor<Connectivity_t>& ELTContainer, BoundToEltType& bound_to_elts);  
  
  //template <typename Connectivity_t>
  //static void getNodeToNodes (const K_FLD::ArrayAccessor<Connectivity_t>& ELTContainer, NodeToNodesType & bound_to_bounds);

  static void getNeighbours (const K_FLD::IntArray& ELTContainer, NeighbourType& neighbors);  

  static inline bool getManifoldNeighbours (const K_FLD::IntArray& ELTContainer, K_FLD::IntArray& neighbors, bool strict=true);
  static inline E_Int getNeighbours (const K_FLD::IntArray& ELTContainer, K_FLD::IntArray& neighbors, bool non_manifold_as_free = false);
   
  inline static void coloring (const K_FLD::IntArray& ELTContainer, const NeighbourType& neighbors,
                               const std::set<BoundaryType> & color_bounds, int_vector_type& colors);
  inline static void coloring (const K_FLD::IntArray& neighbors, int_vector_type& colors);
  inline static void coloring (const ngon_unit& neighbors, int_vector_type& colors);
  
  template<typename T> inline static void coloring_one_connex_heterogeneous (const ngon_unit& neighbors, std::vector<T>& colors, size_t Kseed, T UNSET_COL, T color);
  template<typename T> inline static bool coloring_one_connex_homogeneous (const ngon_unit& neighbors, std::vector<T>& colors, size_t Kseed, T UNSET_COL, T color, T FRONT_COL, T& bad_col);
  template<typename T> inline static bool coloring_one_connex_homogeneous (const ngon_unit& neighbors, std::vector<T>& colors, size_t Kseed, T UNSET_COL, T color, T FRONT_COL);
  template<typename T> inline static bool coloring_one_connex_heterogeneous_if_cols_in_bound(const ngon_unit& neighbors, std::vector<T>& colors, size_t Kseed, T UNSET_COL, T color, E_Int OK_BCOL);

  template<typename T> inline static void coloring_homogeneous_zones_only (const ngon_unit& neighbors, std::vector<T>& colors, T UNSET_COL, T FIRST_COL, T FRONT_COL);
  template<typename T> inline static void coloring_attached_to_col_zones_only(const ngon_unit& neighbors, std::vector<T>& colors, T UNSET_COL, T FIRST_COL, T OK_BCOL);
  template<typename T> inline static void coloring (const ngon_unit& neighbors, std::vector<T>& colors, T UNSET_COL, T FIRST_COL);
  
  inline static void extrapolate (const ngon_unit& neighbors, K_FLD::IntArray& properties);
  
  inline static void smoothd1(const ngon_unit& PHs, const K_FLD::IntArray& noF2E, const std::vector<E_Int>& PHlevel, std::vector<E_Int>& adap_incr); 
  
  static void coloring_pure (const K_FLD::IntArray& neighbors, int_vector_type& colors);
  
  template <typename Connectivity_t>
  static void reversi_connex (const Connectivity_t& conn, const Connectivity_t& neighbors, E_Int Kseed, int_vector_type& orient);
  
  template <typename Connectivity_t>
  static E_Int check_orientation_consistency(const Connectivity_t& PGs, const Connectivity_t& neighbors, std::set<E_Int>& faultys);
  
  /// Applies blindly the swaps provided upon entry
  inline static E_Int fast_swap_edges(std::vector<std::pair<E_Int, E_Int> >& sapwE, K_FLD::IntArray& connectM, K_FLD::IntArray& neighbors);
  
#ifdef DEBUG_ALGO
  static std::set<E_Int> _pool;
#endif

private:

  EltAlgo(void){}

  ~EltAlgo(void){}
};

} // End namespace NUGA

#include "EltAlgo.cxx"

#endif
