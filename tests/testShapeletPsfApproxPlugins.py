#!/usr/bin/env python

#
# LSST Data Management System
# Copyright 2008-2014 LSST Corporation.
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

import unittest
import numpy

import lsst.utils.tests
import lsst.shapelet
import lsst.afw.geom.ellipses
import lsst.afw.table
import lsst.afw.detection
import lsst.meas.multifit
import lsst.meas.base

numpy.random.seed(500)

lsst.pex.logging.Debug("meas.multifit.optimizer.Optimizer", 0)
lsst.pex.logging.Debug("meas.multifit.optimizer.solveTrustRegion", 0)

class ShapeletPsfApproxPluginsTestCase(lsst.utils.tests.TestCase):

    def setUp(self):
        self.psfSigma = 2.0
        self.exposure = lsst.afw.image.ExposureF(41, 41)
        self.exposure.setPsf(lsst.afw.detection.GaussianPsf(19, 19, self.psfSigma))
        self.schema = lsst.afw.table.SourceTable.makeMinimalSchema()
        self.centroidKey = lsst.afw.table.Point2DKey.addFields(self.schema, "centroid", "centroid", "pixels")
        self.schema.getAliasMap().set("slot_Centroid", "centroid")

    def tearDown(self):
        del self.exposure
        del self.schema
        del self.centroidKey

    def checkResult(self, msf):
        # Because we're fitting multiple shapelets to a single Gaussian (a single 0th-order shapelet)
        # we should be able to fit with zero residuals, aside from (single-precision) round-off error.
        dataImage = self.exposure.getPsf().computeImage()
        modelImage = dataImage.Factory(dataImage.getBBox())
        modelImage.getArray()[:,:] *= -1
        msf.evaluate().addToImage(modelImage)
        self.assertClose(dataImage.getArray(), modelImage.getArray(), atol=1E-6, plotOnFailure=False)

    def testSingleFrame(self):
        config = lsst.meas.base.SingleFrameMeasurementTask.ConfigClass()
        config.slots.centroid = None
        config.slots.shape = None
        config.slots.psfFlux = None
        config.slots.apFlux = None
        config.slots.instFlux = None
        config.slots.modelFlux = None
        config.doReplaceWithNoise = False
        config.plugins.names = ["multifit_ShapeletPsfApprox"]
        config.plugins["multifit_ShapeletPsfApprox"].sequence = ["SingleGaussian"]
        task = lsst.meas.base.SingleFrameMeasurementTask(config=config, schema=self.schema)
        measCat = lsst.afw.table.SourceCatalog(self.schema)
        measRecord = measCat.addNew()
        measRecord.set(self.centroidKey, lsst.afw.geom.Point2D(20.0, 20.0))
        task.run(measCat, self.exposure)
        keySingleGaussian = lsst.shapelet.MultiShapeletFunctionKey(
            self.schema["multifit"]["ShapeletPsfApprox"]["SingleGaussian"]
            )
        msfSingleGaussian = measRecord.get(keySingleGaussian)
        self.assertEqual(len(msfSingleGaussian.getComponents()), 1)
        self.checkResult(msfSingleGaussian)

    def testForced(self):
        config = lsst.meas.base.ForcedMeasurementTask.ConfigClass()
        config.slots.centroid = "base_TransformedCentroid"
        config.slots.shape = None
        config.slots.psfFlux = None
        config.slots.apFlux = None
        config.slots.instFlux = None
        config.slots.modelFlux = None
        config.doReplaceWithNoise = False
        config.plugins.names = ["base_TransformedCentroid", "multifit_ShapeletPsfApprox"]
        config.plugins["multifit_ShapeletPsfApprox"].sequence = ["SingleGaussian"]
        refCat = lsst.afw.table.SourceCatalog(self.schema)
        refRecord = refCat.addNew()
        refRecord.set(self.centroidKey, lsst.afw.geom.Point2D(20.0, 20.0))
        refWcs = self.exposure.getWcs() # same as measurement Wcs
        task = lsst.meas.base.ForcedMeasurementTask(config=config, refSchema=self.schema)
        measCat = task.run(self.exposure, refCat, refWcs).sources
        measRecord = measCat[0]
        measSchema = measCat.schema
        keySingleGaussian = lsst.shapelet.MultiShapeletFunctionKey(
            measSchema["multifit"]["ShapeletPsfApprox"]["SingleGaussian"]
            )
        msfSingleGaussian = measRecord.get(keySingleGaussian)
        self.assertEqual(len(msfSingleGaussian.getComponents()), 1)
        self.checkResult(msfSingleGaussian)

def suite():
    """Returns a suite containing all the test cases in this module."""

    lsst.utils.tests.init()

    suites = []
    suites += unittest.makeSuite(ShapeletPsfApproxPluginsTestCase)
    suites += unittest.makeSuite(lsst.utils.tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests"""
    lsst.utils.tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
