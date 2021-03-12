/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#ifndef NUGA_SMOOTHER_HXX
#define NUGA_SMOOTHER_HXX

#include "Nuga/include/subdivision.hxx"


namespace NUGA
{
  enum eSmoother { V1_NEIGH = 0, SHELL = 1 };

/// Base smoother class
template <typename mesh_t>
struct smoother
{
  using output_t = typename sensor_output_data<mesh_t::SUBTYPE>::type;
  using cell_adap_incr_t = typename output_t::cell_output_type;
     
  smoother() = default;

  virtual void smooth(const mesh_t& hmesh, cell_adap_incr_t& adap_incr)  = 0;
  
  virtual ~smoother() {}
};

}
#endif
