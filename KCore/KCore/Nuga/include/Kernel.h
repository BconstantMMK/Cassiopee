/*



--------- NUGA v1.0



*/
//Authors : Sâm Landier (sam.landier@onera.fr)

#ifndef __GENERATOR_DELAUNAY_KERNEL_H__
#define __GENERATOR_DELAUNAY_KERNEL_H__

#include "delaunay_preds.h"
#include "MeshData.h"
#include "Nuga/include/MeshTool.h"
#include "macros.h"

#include <map>

namespace DELAUNAY
{

typedef   float                                    CONSTRAINED ;
typedef   int                                      UNCONSTRAINED ;

template <typename T>
class Kernel
{

 public: /** Typedefs */

    typedef   NUGA::size_type                    size_type;
    typedef   NUGA::int_vector_type              int_vector_type;
    typedef   NUGA::int_set_type                 int_set_type;
    typedef   NUGA::int_pair_type                int_pair_type;
    typedef   NUGA::int_pair_vector_type         int_pair_vector_type;
    typedef   NUGA::int_pair_set_type            int_pair_set_type;
    typedef   NUGA::bool_vector_type             bool_vector_type;
    typedef   NUGA::non_oriented_edge_set_type   non_oriented_edge_set_type;

    typedef   Kernel                                   self_type;
    typedef   K_MESH::Triangle                         element_type;

    typedef   no_predicate<K_MESH::NO_Edge>            unconstrained_predicate;
    typedef   HardEdgeCriterion                        constrained_predicate;
        
public:

  /// Constructor.
  explicit Kernel(MeshData& data, const NUGA::MeshTool& tool);
 
         
  ///
  ~Kernel(void);

  ///
  template <typename ConstraintType>
  E_Int insertNode(size_type N, const T& m, const ConstraintType& dummy);

  /// sets the hard edges
  void setConstraint(const NUGA::non_oriented_edge_set_type& hard_edges);

private:

  /// Gets the cavity associated to the input node N regarding the metric value m.
  /** Here m is evaluated at N but a more sophisticated approach could be to take a mean of the triangle summits.*/
  template <typename isConstrained>
  E_Int __getCavity(size_type N, const T& m, const K_FLD::FloatArray& pos,
                    const K_FLD::IntArray& neighbors, const int_vector_type& ancestors, int_set_type& cavity,
                    int_pair_vector_type& cboundary);

  /// Remeshes the cavity
  E_Int __remeshCavity(size_type N, K_FLD::IntArray & connect, K_FLD::IntArray& neighbors,
                      int_vector_type& ancestors, const int_set_type& cavity,
                      const int_pair_vector_type& cboundary);

  /// Invalidates old elements of the remeshed cavity.
  void __invalidCavityElements(const int_set_type& cavity, const K_FLD::IntArray& connect,
                               bool_vector_type& mask);

  ///
  template <typename isConstrained>
  E_Int __getInitialCavity(size_type N, 
                           const K_FLD::FloatArray& pos, const K_FLD::IntArray& connect, 
                           const K_FLD::IntArray& neighbors,
                           const int_vector_type& ancestors, int_set_type& base, int_set_type& cavity,
                           int_pair_set_type& cboundary);

  ///
  template <typename isConstrained>
  inline
  void __appendCavity(const K_FLD::IntArray& neighbors, const int_set_type& base,
                      int_set_type& cavity, int_pair_set_type& cboundary);

  ///
  E_Int __fixCavity(size_type N, const K_FLD::FloatArray& pos, const K_FLD::IntArray& connectM, const K_FLD::IntArray& neighbors,
                    const int_vector_type& ancestors, const int_set_type& base, int_set_type& cavity,
                    int_pair_set_type& cboundary);

  ///
  E_Int __ensureEmptyCavity(const K_FLD::IntArray& connectM, const K_FLD::IntArray& neighbors, const int_vector_type& ancestors, 
                            const int_set_type& base, int_set_type& cavity, int_pair_set_type& cboundary);

  ///
  E_Int __ensureStarShape(size_type N, const K_FLD::FloatArray& pos, const K_FLD::IntArray& connectM, 
                          const K_FLD::IntArray& neighbors, const int_set_type& base,
                          int_set_type& cavity, int_pair_set_type& cboundary);

  ///
  void __getSortedBoundary(const K_FLD::IntArray& connectM, const K_FLD::IntArray& neighbors, size_type Ki, size_type b0,
                           const int_set_type& cavity, int_pair_set_type& cboundary,
                           int_set_type& visitedK, int_pair_vector_type& sorted_boundary); 

  E_Int __getSortedBoundary2(const K_FLD::IntArray& connect, int_pair_set_type& in, int_pair_vector_type& out);

  ///
  void __getBoundary(int_set_type& cavity, const K_FLD::IntArray& neighbors, int_pair_set_type& cboundary);

private:

  MeshData& _data;
  const NUGA::MeshTool& _tool;
  DelaunayCriterion<T>              _Ball_pred;
  unconstrained_predicate           _unconstrained_pred;
  constrained_predicate             *_constrained_pred;

  // Temporary containers (put here for speed).
  int_set_type          _cavity;
  int_pair_vector_type  _cboundary;
  int_set_type           _base;
  int_pair_set_type     _sbound, _real_cboundary;
  int_set_type          _visited;
  int_set_type          inodes;
  int_vector_type       Ancs;
  int_vector_type       elements;
  std::map<size_type, int_pair_set_type::iterator> _node_to_rightS;
  
public:
 size_type _Nmatch;
#ifdef E_TIME
public:
  double inval_time;
  double remesh_time;
  double cavity_time;
  double init_cavity_time;
  double sorting_bound_time;
  double fix_cavity_time;
  double _base_time;
  double _append_time;
#endif

};

}

#include "Kernel.cxx"

#endif

