# - getNbEdges (pyTree) -
import OCC.PyTree as OCC

hook = OCC.readCAD("cube.step", "fmt_step")
print(OCC.getNbEdges(hook))
#> 12
