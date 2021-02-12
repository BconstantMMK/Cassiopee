/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#pragma once

#include "Nuga/include/DynArray.h"
#include <vector>


class SwapperT3
{
public:

  enum eDegenType { OK=0, HAT = 1, SPIKE = 2, SMALL = 4};

  //lambdac is the critical lambda to distinguish hat and spikes
  static eDegenType degen_type2(const K_FLD::FloatArray& crd, E_Int N0, E_Int N1, E_Int N2, E_Float tol2, E_Float lambdac, E_Int& n);

  static E_Int run
    (const K_FLD::FloatArray& coord, E_Float tol, K_FLD::IntArray& connect, std::vector<E_Int>& oids);

  static E_Int clean(const K_FLD::FloatArray& coord, E_Float tol, K_FLD::IntArray& connect, std::vector<E_Int> & coids, std::vector<E_Int> & cnids);

  static E_Int remove_degen(K_FLD::IntArray& connect, std::vector<E_Int>& cnids);

  static bool has_degen(const K_FLD::FloatArray& coord, const K_FLD::IntArray& connect, E_Float tol2);
  static bool has_hat(const K_FLD::FloatArray& coord, const K_FLD::IntArray& connect, E_Float tol2);
  static void edit_T3_caracs(const K_FLD::FloatArray& crd, E_Int* pN);
  
  //
  SwapperT3();
  ~SwapperT3();
};

