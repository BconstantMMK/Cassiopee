/*



--------- NUGA v1.0



*/
//Authors : S�m Landier (sam.landier@onera.fr)

#include "Nuga/include/TSSplitter.h"
#include "Nuga/include/Triangle.h"
#include "Nuga/include/ContourSplitter.h"

// Splits the connectivity (only) with the edge N0N1 and stores the bits 
// into separate containers.
void TSSplitter::split
(const K_FLD::IntArray& BAR, const K_FLD::IntArray& polyLine, std::vector<K_FLD::IntArray> & connectBout)
{
  std::set<K_MESH::NO_Edge> cuttingEdges;
  for (E_Int Si = 0; Si < polyLine.cols(); ++Si)
    cuttingEdges.insert(polyLine.col(Si));

  ContourSplitter<K_MESH::Triangle, K_MESH::NO_Edge>::splitConnectivity(BAR, cuttingEdges, connectBout);
}

// Splits the connectivity with the edge N0N1 and stores the connectivity 
// bits and corresponding coordinates into separate containers.
void TSSplitter::split
(const K_FLD::FloatArray& pos, const K_FLD::IntArray& connectBin, const K_FLD::IntArray& polyLine,
 std::vector<K_FLD::FloatArray> & posOut, std::vector<K_FLD::IntArray> & connectBout)
{
  split(connectBin, polyLine, connectBout);

  ContourSplitter<K_MESH::Triangle, K_MESH::NO_Edge>::splitCoordinates(connectBout, pos, posOut);
}
