/*    
    Copyright 2013-2025 Onera.

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
#ifndef _CPLOT_TESSELATIONEVALUATIONSHADER_HPP_
#define _CPLOT_TESSELATIONEVALUATIONSHADER_HPP_
#include "ShaderObject.h"

namespace CPlot
{
  class TesselationEvaluationShader : public ShaderObject
  {
  public:
    TesselationEvaluationShader();
    TesselationEvaluationShader( const std::string& src );
    TesselationEvaluationShader(const TesselationEvaluationShader& shader) = delete;
    virtual ~TesselationEvaluationShader();

    TesselationEvaluationShader& operator = (const TesselationEvaluationShader& obj) = delete;

  };
}

#endif
