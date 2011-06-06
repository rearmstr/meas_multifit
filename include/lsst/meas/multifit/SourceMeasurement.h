// -*- LSST-C++ -*-
/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010, 2011 LSST Corporation.
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

#ifndef LSST_MEAS_MULTIFIT_SOURCE_MEASUREMENT_H
#define LSST_MEAS_MULTIFIT_SOURCE_MEASUREMENT_H

#include "lsst/afw/detection/Measurement.h"
#include "lsst/afw/detection/Photometry.h"
#include "lsst/afw/detection/Astrometry.h"
#include "lsst/afw/detection/Shape.h"
#include "lsst/meas/multifit/GaussNewtonOptimizer.h"
#include "lsst/meas/multifit/BruteForceSourceOptimizer.h"
#include <Eigen/Core>

namespace lsst {
namespace meas {
namespace multifit {

template <
    int basisSize ///< Number of basis functions; instantiated for 2, 8, and 17 to match persisted basis sets.
    >
class ShapeletModelPhotometry : public lsst::afw::detection::Photometry {
public:
    typedef lsst::afw::detection::Schema Schema;
    typedef lsst::afw::detection::SchemaEntry SchemaEntry;
    typedef lsst::afw::detection::Photometry Base;
    typedef lsst::afw::detection::Measurement<Base> Measurement;

    enum {
        FLUX = Base::FLUX,
        FLUX_ERR,
        STATUS,
        E1, E2, RADIUS, 
        COEFFICIENTS
    };

    enum {
        NO_EXPOSURE=0x01, 
        NO_PSF=0x02, 
        NO_SOURCE=0x04, 
        NO_BASIS=0x08,
        NO_FOOTPRINT=0x10, 
        BAD_INITIAL_MOMENTS=0x20, 
        OPTIMIZER_FAILED=0x40,
        GALAXY_MODEL_FAILED=0x80
    };

    virtual void defineSchema(lsst::afw::detection::Schema::Ptr schema);

    static bool doConfigure(lsst::pex::policy::Policy const& policy);

    template <typename ExposureT>
    static Photometry::Ptr doMeasure(CONST_PTR(ExposureT) im,
                                     CONST_PTR(afw::detection::Peak),
                                     CONST_PTR(afw::detection::Source)
                                    );
    ShapeletModelPhotometry(int const status);
#if 0
    ShapeletModelPhotometry(
        GaussNewtonOptimizer & optimizer,
        BaseEvaluator::Ptr const & evaluator
    );
#endif
    ShapeletModelPhotometry(
        Evaluator::Ptr const & evaluator,
        ndarray::Array<double const, 1,1> const & param,
        ndarray::Array<double const, 1,1> const & coeff,
        ndarray::Array<double const, 2,1> const & covar,
        int const status
    );

    static bool usePixelWeights;
    static bool fitDeltaFunction;
    static bool isEllipticityActive, isRadiusActive, isPositionActive;
    static lsst::afw::image::MaskPixel bitmask;
    static int nGrowFp;
    static int nCoeff;
    static ModelBasis::Ptr basis;

    static int nTestPoints;
#if 0
    static int maxIter
    static double ftol, gtol, minStep, tau;
    static bool retryWithSvd;
#endif

private:



    ShapeletModelPhotometry() : lsst::afw::detection::Photometry() {init();}
    LSST_SERIALIZE_PARENT(lsst::afw::detection::Photometry);
};

}}}

LSST_REGISTER_SERIALIZER(lsst::meas::multifit::ShapeletModelPhotometry<2>);
LSST_REGISTER_SERIALIZER(lsst::meas::multifit::ShapeletModelPhotometry<8>);
LSST_REGISTER_SERIALIZER(lsst::meas::multifit::ShapeletModelPhotometry<17>);

#endif
