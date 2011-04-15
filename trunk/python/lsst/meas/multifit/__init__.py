# 
# LSST Data Management System
# Copyright 2008, 2009, 2010, 2011 LSST Corporation.
# 
# This product includes software developed by the
# LSST Project (http://www.lsst.org/).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the LSST License Statement and 
# the GNU General Public License along with this program.  If not, 
# see <http://www.lsstcorp.org/LegalNotices/>.
#
import lsst.afw.geom.ellipses

from multifitLib import (
    BaseEvaluator,
    Evaluator,
    ModelBasis,
    ShapeletModelBasis,
    CompoundShapeletModelBasis,
    CompoundShapeletBuilder,
    )
import multifitLib
import os
import eups

from . import sampling

Ellipticity = lsst.afw.geom.ellipses.ConformalShear
Radius = lsst.afw.geom.ellipses.TraceRadius
EllipseCore = lsst.afw.geom.ellipses.Separable[(Ellipticity, Radius)];
CompoundShapeletBuilder.ComponentVector = multifitLib.CompoundShapelet_ComponentVector
CompoundShapeletModelBasis.ComponentVector = multifitLib.CompoundShapelet_ComponentVector

def loadBasis(name):
    path = os.path.join(eups.productDir("meas_multifit"), "data", "%s.boost" % name)
    return CompoundShapeletModelBasis.load(path)