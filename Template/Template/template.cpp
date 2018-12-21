/*    
    Copyright 2013-2019 Onera.

    This file is part of Cassiopee.

    Cassiopee is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cassiopee is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cassiopee.  If not, see <http://www.gnu.org/licenses/>.
*/

// C'est le fichier correspondant au module et aux fonctions qu'il contient.
// Il faut changer partout "template" en nom du module.

#define K_ARRAY_UNIQUE_SYMBOL
#include "template.h"

// ============================================================================
/* Dictionnary of all functions of the python module */
// ============================================================================
static PyMethodDef Pytemplate [] =
{
  {"intFloatExample", K_TEMPLATE::intFloatExample, METH_VARARGS},
  {"numpyExample", K_TEMPLATE::numpyExample, METH_VARARGS},
  {"numpyExample2", K_TEMPLATE::numpyExample2, METH_VARARGS},
  {"arrayExample", K_TEMPLATE::arrayExample, METH_VARARGS},
  {"pyTreeExample", K_TEMPLATE::pyTreeExample, METH_VARARGS},
  {"pyTreeExample1", K_TEMPLATE::pyTreeExample1, METH_VARARGS},
  {NULL, NULL}
};

// ============================================================================
/* Init of module */
// ============================================================================
extern "C"
{
  PyMODINIT_FUNC inittemplate();
  PyMODINIT_FUNC inittemplate()
  {
    Py_InitModule("template", Pytemplate);
    import_array();
  }
}
