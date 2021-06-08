/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#ifndef NUGA_POLYHEDRON_HXX
#define NUGA_POLYHEDRON_HXX

#include "Nuga/include/Polyhedron.h"
#include "Nuga/include/Triangulator.h"

namespace NUGA
{
///autonomous (i.e. self contained) polyhedron
template <int TopoShape>
struct aPolyhedron : public K_MESH::Polyhedron<TopoShape>
{
  using parent_type = K_MESH::Polyhedron<TopoShape>;
  
  ngon_unit          m_pgs;
  std::vector<E_Int> m_faces;
  K_FLD::FloatArray  m_crd;
  E_Float            m_Lref2;
  E_Int              m_oriented;
  mutable E_Float    m_normal[3];
  mutable E_Float    m_centroid[3];
  
  aPolyhedron();
  aPolyhedron(const ngon_unit* pgs, const E_Int* faces, E_Int nb_faces, const K_FLD::FloatArray& crd); // from "mesh" to autonmous
  aPolyhedron(const parent_type& ph, const K_FLD::FloatArray& crd); // from "mesh" to autonmous
  aPolyhedron(const ngon_unit& lpgs, const K_FLD::FloatArray& crd);

  aPolyhedron(const parent_type& ph, const K_FLD::FloatArray& crd, E_Float L2r);
 
  aPolyhedron(const aPolyhedron& r);

  aPolyhedron(aPolyhedron&& r);
    
  aPolyhedron& operator=(const parent_type& rhs) = delete;
  aPolyhedron& operator=(const aPolyhedron& rhs);
  aPolyhedron& operator=(aPolyhedron&& rhs);
          
  void set(const parent_type& ph, const K_FLD::FloatArray& crd);

  E_Int* faces() {return &m_faces[0];} //this class can change this inner attr, but cannot touch from parent class (so const version only avail. from there)
  
  void reverse();
  void reorient(bool outward);
  
  void clear() {m_pgs.clear(); m_faces.clear(); m_crd.clear();}
  bool empty() { return m_faces.empty(); }
  
  template<typename TriangulatorType> E_Int volume(E_Float& v, bool need_reorient) const;
  
  double extent() const; //wrapper to volume, to be generic surf/vol
  double metrics() ; //same as extent : no normal computation as in surfacic

  using parent_type::triangulate; //to make it visible (!!)
  using parent_type::Lref2;       //to make it visible (!!)

  template <typename TriangulatorType>
  E_Int triangulate(const TriangulatorType& dt) const;

  E_Int cvx_triangulate(const K_FLD::FloatArray& dummy) const;

  const double* get_centroid() const; // WARNING : currently implemented as iso_bary

  double Lref2() const { return (m_Lref2 > 0.) ? m_Lref2 : parent_type::Lref2(m_crd); } // if passed by mesh_t, return it, otherwise compute it first

  void plug() { parent_type::_pgs = &m_pgs; parent_type::_faces = &m_faces[0]; parent_type::_nb_faces = m_faces.size(); }
};

///
template <int TopoShape>
aPolyhedron<TopoShape>::aPolyhedron() :parent_type(), m_Lref2(NUGA::FLOAT_MAX), m_oriented(0)
{
  m_normal[0] = m_centroid[0] = NUGA::FLOAT_MAX;
  //plug();
}

///
template <int TopoShape>
aPolyhedron<TopoShape>::aPolyhedron(const ngon_unit* pgs, const E_Int* faces, E_Int nb_faces, const K_FLD::FloatArray& crd)
  :m_Lref2(NUGA::FLOAT_MAX), m_oriented(0)
{
  parent_type ph(pgs, faces, nb_faces);
  set(ph, crd);
}

///
template <int TopoShape>
aPolyhedron<TopoShape>::aPolyhedron(const parent_type& ph, const K_FLD::FloatArray& crd)
  :m_Lref2(NUGA::FLOAT_MAX), m_oriented(0)
{
  set(ph, crd);
  m_normal[0] = m_centroid[0] = NUGA::FLOAT_MAX;
}

///
template <int TopoShape>
aPolyhedron<TopoShape>::aPolyhedron(const ngon_unit& lpgs, const K_FLD::FloatArray& lcrd)
  :m_pgs(lpgs), m_crd(lcrd), m_Lref2(NUGA::FLOAT_MAX), m_oriented(0)
{
  
  m_faces.clear();
  K_CONNECT::IdTool::init_inc(m_faces, m_pgs.size(), 1);

  m_normal[0] = m_centroid[0] = NUGA::FLOAT_MAX;
  
  plug();
}

///
template <int TopoShape>
aPolyhedron<TopoShape>::aPolyhedron(const parent_type& ph, const K_FLD::FloatArray& crd, E_Float L2r) :aPolyhedron(ph, crd)
{
  m_Lref2 = L2r;
  m_oriented = 0;
  m_normal[0] = m_centroid[0] = NUGA::FLOAT_MAX;
}

template <int TopoShape>
aPolyhedron<TopoShape>::aPolyhedron(const aPolyhedron& r) :m_pgs(r.m_pgs), m_faces(r.m_faces), m_crd(r.m_crd), m_Lref2(NUGA::FLOAT_MAX), m_oriented(0)
{ 
  m_normal[0] = m_centroid[0] = NUGA::FLOAT_MAX;

  plug();
}

template <int TopoShape>
aPolyhedron<TopoShape>::aPolyhedron (aPolyhedron<TopoShape>&& rhs) :/* = default; rejected by old compiler intel (15)*/
  parent_type(rhs), m_pgs(std::move(rhs.m_pgs)), m_faces(std::move(rhs.m_faces)), m_crd(std::move(rhs.m_crd)), m_Lref2(rhs.m_Lref2), m_oriented(rhs.m_oriented)
{
  m_normal[0] = rhs.m_normal[0];
  m_normal[1] = rhs.m_normal[1];
  m_normal[2] = rhs.m_normal[2];

  m_centroid[0] = rhs.m_centroid[0];
  m_centroid[1] = rhs.m_centroid[1];
  m_centroid[2] = rhs.m_centroid[2];

  plug();
}

template <int TopoShape>
aPolyhedron<TopoShape>& aPolyhedron<TopoShape>::operator=(const aPolyhedron& rhs)
{
  m_pgs = rhs.m_pgs;
  m_faces = rhs.m_faces;
  m_crd = rhs.m_crd;
  m_Lref2 = rhs.m_Lref2;
  m_oriented = rhs.m_oriented;

  parent_type::_triangles = rhs._triangles;

  m_normal[0] = rhs.m_normal[0];
  m_normal[1] = rhs.m_normal[1];
  m_normal[2] = rhs.m_normal[2];

  m_centroid[0] = rhs.m_centroid[0];
  m_centroid[1] = rhs.m_centroid[1];
  m_centroid[2] = rhs.m_centroid[2];

  plug();

  return *this;
}

template <int TopoShape>
aPolyhedron<TopoShape>& aPolyhedron<TopoShape>::operator=(aPolyhedron&& rhs)
{
  m_pgs = std::move(rhs.m_pgs);
  m_faces = std::move(rhs.m_faces);
  m_crd = std::move(rhs.m_crd);
  m_Lref2 = rhs.m_Lref2;
  m_oriented = rhs.m_oriented;

  parent_type::_triangles = rhs._triangles;

  m_normal[0] = rhs.m_normal[0];
  m_normal[1] = rhs.m_normal[1];
  m_normal[2] = rhs.m_normal[2];

  m_centroid[0] = rhs.m_centroid[0];
  m_centroid[1] = rhs.m_centroid[1];
  m_centroid[2] = rhs.m_centroid[2];

  rhs._pgs = nullptr;
  rhs._faces = nullptr;
  rhs._nb_faces = 0;
  rhs._triangles = nullptr;
  rhs.m_normal[0] = rhs.m_centroid[0] = NUGA::FLOAT_MAX;

  plug();

  return *this;
}

///
template <int TopoShape>
void aPolyhedron<TopoShape>::set(const parent_type& ph, const K_FLD::FloatArray& crd)
{
  ph.compact(crd, m_pgs, m_crd);
  m_faces.clear();
  K_CONNECT::IdTool::init_inc(m_faces, m_pgs.size(), 1);
  
  plug();
}

template <int TopoShape>
void aPolyhedron<TopoShape>::reverse()
{
  for (E_Int i = 0; i < m_pgs.size(); ++i)
  {
    E_Int s = m_pgs.stride(i);
    E_Int* p = m_pgs.get_facets_ptr(i);
    std::reverse(p, p + s);
  }
  if (m_oriented != 0) m_oriented = !m_oriented;
}

///
template <int TopoShape>
void aPolyhedron<TopoShape>::reorient(bool outward)
{
  if (m_oriented != 0) // already oriented
  {
    if (outward && m_oriented == 1) return;
    if (!outward && m_oriented == -1) return;

    reverse();
    m_oriented = !m_oriented;
    return;
  }
  
  ngon_unit neighbors;
  K_MESH::Polygon::build_pg_neighborhood(m_pgs, neighbors);
  std::vector<E_Int> oids, orient(m_faces.size(), 1);
  NUGA::EltAlgo<K_MESH::Polygon>::reversi_connex(m_pgs, neighbors, 0/*ref*/, orient);

  //Apply new orientation
  for (E_Int i = 0; i < m_pgs.size(); ++i)
  {
    if (orient[i] == -1)
    {
      E_Int s = m_pgs.stride(i);
      E_Int* p = m_pgs.get_facets_ptr(i);
      std::reverse(p, p + s);
    }
  }

  double v;
  volume<DELAUNAY::Triangulator>(v, false);

  bool need_reverse = (v < 0. && outward) || (v > 0. && !outward);

  if (need_reverse)
    reverse();

  m_oriented = outward ? 1 : -1;
}

template <int TopoShape>
template <typename TriangulatorType>
E_Int aPolyhedron<TopoShape>::triangulate(const TriangulatorType& dt) const
{
  return parent_type::triangulate(dt, m_crd);
}

template <int TopoShape>
E_Int aPolyhedron<TopoShape>::cvx_triangulate(const K_FLD::FloatArray& dummy) const
{
  return parent_type::cvx_triangulate(m_crd);
}


template <int TopoShape>
const double* aPolyhedron<TopoShape>::get_centroid() const 
{ 
  if (m_centroid[0] == NUGA::FLOAT_MAX)
  {
    for (size_t d = 0; d < 3; ++d) m_centroid[d] = 0.;
    for (E_Int i = 0; i < m_crd.cols(); ++i)
    {
      const E_Float* p = m_crd.col(i);
      for (size_t d = 0; d < 3; ++d) m_centroid[d] += p[d];
    }
    E_Float k = 1. / (E_Float)m_crd.cols();
    for (size_t i = 0; i < 3; ++i) m_centroid[i] *= k;
  }
  return m_centroid;
}

template <int TopoShape>
template<typename TriangulatorType>
E_Int aPolyhedron<TopoShape>::volume(E_Float& v, bool need_reorient) const
{
  return parent_type::template volume<TriangulatorType>(m_crd, v, need_reorient);
}

template <int TopoShape>
double aPolyhedron<TopoShape>::extent() const
{
  double v(0.);
  volume<DELAUNAY::Triangulator>(v, true/*reorient*/);//fixme : avoid systematic reorient
  return ::fabs(v);
}

template <int TopoShape>
double aPolyhedron<TopoShape>::metrics()
{
  return extent();
}


// autonomous with history
template <int TopoShape>
struct haPolyhedron : public aPolyhedron<TopoShape>
{
  using parent_type = NUGA::aPolyhedron<TopoShape>;
  using base_type = K_MESH::Polyhedron<TopoShape>;
  
  std::vector<E_Int> poids;

  haPolyhedron():parent_type() {};
  
  haPolyhedron& operator=(const base_type& rhs) = delete; // crd must be also specified so disable this operation (permanent)
  haPolyhedron& operator=(const parent_type& rhs) = delete;// {set(rhs, rhs.m_crd);} delet until required
  haPolyhedron& operator=(const haPolyhedron& rhs);
          
  void set(const base_type& ph, const K_FLD::FloatArray& crd);
  template <typename ngo_t> void set(ngo_t& ng, E_Int PHi, const K_FLD::FloatArray& crd); //template to avoid cycling with ngon_t.hxx

  bool is_closed(std::set<K_MESH::NO_Edge>& free_edges);
  void get_edge_refinement
  (const std::set<K_MESH::NO_Edge>& free_edges, E_Int idx_start, std::map<K_MESH::NO_Edge, std::deque<E_Int> >& edge_to_refine_nodes);
  
};

///
template <int TopoShape>
haPolyhedron<TopoShape>& haPolyhedron<TopoShape>::operator=(const haPolyhedron<TopoShape>& rhs)
{
  poids = rhs.poids;
  parent_type::m_pgs = rhs.m_pgs;
  parent_type::m_faces = rhs.m_faces;
  parent_type::m_crd = rhs.m_crd;
  
  parent_type::plug();

  return *this;
}

///
template <int TopoShape>
void haPolyhedron<TopoShape>::set(const base_type& ph, const K_FLD::FloatArray& crd)
{
  ph.compact(crd, parent_type::m_pgs, parent_type::m_crd, &poids);
  parent_type::m_faces.clear();
  K_CONNECT::IdTool::init_inc(parent_type::m_faces, parent_type::m_pgs.size(), 1);
  
  parent_type::plug();
}

///
template <int TopoShape>
template <typename ngo_t>
void haPolyhedron<TopoShape>::set(ngo_t& ng, E_Int PHi, const K_FLD::FloatArray& crd)
{
  base_type e(ng,PHi);
  set(e, crd);
}

}

#endif
