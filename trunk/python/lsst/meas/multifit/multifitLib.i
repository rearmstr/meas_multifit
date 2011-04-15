// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
%define multifitLib_DOCSTRING
"
Basic routines to talk to lsst::meas::multifit classes
"
%enddef


%feature("autodoc", "1");
%module(package="lsst.meas.multifit", docstring=multifitLib_DOCSTRING) multifitLib

// Suppress swig complaints
#pragma SWIG nowarn=314                 // print is a python keyword (--> _print)
#pragma SWIG nowarn=362                 // operator=  ignored
#pragma SWIG nowarn=401                 // nothin known about base class X
%{
#include "lsst/afw/detection.h"
#include "lsst/meas/multifit/constants.h"
#include "lsst/meas/multifit/BaseEvaluator.h"
#include "lsst/meas/multifit/Evaluator.h"
#include "lsst/meas/multifit/ModelBasis.h"
#include "lsst/meas/multifit/ShapeletModelBasis.h"
#include "lsst/meas/multifit/CompoundShapeletModelBasis.h"
#define PY_ARRAY_UNIQUE_SYMBOL LSST_MEAS_MULTIFIT_NUMPY_ARRAY_API
#include "numpy/arrayobject.h"
#include "lsst/ndarray/python.h"
#include "lsst/ndarray/python/eigen.h"
%}

%inline %{
namespace boost {}
namespace lsst { 
    namespace afw {
        namespace image {}
        namespace detection {}
        namespace math {}
        namespace geom {}
    }
    namespace meas { namespace multifit {} }
}

using namespace lsst;
using namespace lsst::meas::multifit;
%}

/******************************************************************************/
%init %{
    import_array();
%}

%include "lsst/p_lsstSwig.i"
%include "lsst/base.h"
%include "std_complex.i"

%lsst_exceptions();

%pythoncode %{
import lsst.utils

def version(HeadURL = r"$HeadURL$"):
    """Return a version given a HeadURL string. If a different version is setup, return that too"""

    version_svn = lsst.utils.guessSvnVersion(HeadURL)

    try:
        import eups
    except ImportError:
        return version_svn
    else:
        try:
            version_eups = eups.getSetupVersion("meas_multifit")
        except AttributeError:
            return version_svn

    if version_eups == version_svn:
        return version_svn
    else:
        return "%s (setup: %s)" % (version_svn, version_eups)
%}

%include "lsst/ndarray/ndarray.i"
%import "lsst/afw/geom/geomLib.i"
%import "lsst/afw/geom/ellipses/ellipsesLib.i"
%import "lsst/afw/detection/detectionLib.i"
%import "lsst/afw/math/mathLib.i"
%import "lsst/afw/math/shapelets/shapeletsLib.i"
%import "lsst/afw/image/imageLib.i"

/*****************************************************************************/
%declareNumPyConverters(lsst::ndarray::Array<lsst::meas::multifit::Pixel, 2, 1>);
%declareNumPyConverters(lsst::ndarray::Array<lsst::meas::multifit::Pixel const, 2, 1>);
%declareNumPyConverters(lsst::ndarray::Array<lsst::meas::multifit::Pixel, 2, 2>);
%declareNumPyConverters(lsst::ndarray::Array<lsst::meas::multifit::Pixel const, 2, 2>);
%declareNumPyConverters(lsst::ndarray::Array<lsst::meas::multifit::Pixel, 3, 3>);
%declareNumPyConverters(lsst::ndarray::Array<lsst::meas::multifit::Pixel const, 3, 3>);
%declareNumPyConverters(lsst::ndarray::Array<lsst::meas::multifit::Pixel, 1, 1>);
%declareNumPyConverters(lsst::ndarray::Array<lsst::meas::multifit::Pixel const, 1, 1>);
%declareNumPyConverters(lsst::ndarray::Array<double const, 1, 1>);
%declareNumPyConverters(lsst::ndarray::Array<double, 1, 1>);
%declareNumPyConverters(lsst::ndarray::Array<double, 2, 2>);

%include "lsst/meas/multifit/constants.h"

SWIG_SHARED_PTR(ModelBasisPtr, lsst::meas::multifit::ModelBasis);
SWIG_SHARED_PTR_DERIVED(ShapeletModelBasisPtr, lsst::meas::multifit::ModelBasis,
        lsst::meas::multifit::ShapeletModelBasis);
SWIG_SHARED_PTR_DERIVED(CompoundShapeletModelBasisPtr, lsst::meas::multifit::ModelBasis,
        lsst::meas::multifit::CompoundShapeletModelBasis);

%nodefaultctor lsst::meas::multifit::ModelBasis;
%nodefaultctor lsst::meas::multifit::ShapeletModelBasis;
%nodefaultctor lsst::meas::multifit::CompoundShapeletModelBasis;


%extend lsst::meas::multifit::CompoundShapeletModelBasis {
    %feature("shadow") _getForward %{
        def getForward(self):
            return $action(self)
    %}
    %feature("shadow") _getReverse %{
        def getReverse(self):
            return $action(self)
    %}
    %feature("shadow") _extractComponents %{
        def extractComponents(self):
            return $action(self)
    %}


    lsst::ndarray::Array<lsst::meas::multifit::Pixel const, 2, 1> _getForward() const {
        return self->getForward();
    }
    lsst::ndarray::Array<lsst::meas::multifit::Pixel const, 2, 1> _getReverse() const {
        return self->getReverse();
    }
    lsst::meas::multifit::CompoundShapeletModelBasis::ComponentVector _extractComponents() const {
        return self->extractComponents();
    }
};

%include "lsst/meas/multifit/ModelBasis.h"
%include "lsst/meas/multifit/ShapeletModelBasis.h"
%include "lsst/meas/multifit/CompoundShapeletModelBasis.h"

%template(CompoundShapelet_ComponentVector) std::vector<boost::shared_ptr<lsst::meas::multifit::ShapeletModelBasis> >;





SWIG_SHARED_PTR(BaseEvaluatorPtr, lsst::meas::multifit::BaseEvaluator);

%include "lsst/meas/multifit/BaseEvaluator.h"

SWIG_SHARED_PTR_DERIVED(EvaluatorPtr, lsst::meas::multifit::BaseEvaluator, 
        lsst::meas::multifit::Evaluator);

%include "lsst/meas/multifit/Evaluator.h"

%template(make) lsst::meas::multifit::Evaluator::make<double>;
%template(make) lsst::meas::multifit::Evaluator::make<float>;