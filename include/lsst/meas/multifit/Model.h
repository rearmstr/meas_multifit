#ifndef LSST_MEAS_MULTIFIT_MODEL_H
#define LSST_MEAS_MULTIFIT_MODEL_H

#include <Eigen/Core>

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/weak_ptr.hpp>

#include <ndarray_fwd.hpp>

#include "lsst/meas/multifit/core.h"
#include "lsst/afw/geom/Box.h"

namespace lsst{
namespace meas {
namespace multifit {

//forward declarations
class ModelProjection;
class ModelFactory;

/**
 *  \brief A model for an astronomical object (ABC).
 *
 *  A Model's parameters are always in "global" (typically celestial) coordinates.
 *  A projection of the Model to a particular image coordinate system (along with
 *  convolution by the appropriate PSF and application of other observational effects)
 *  is represented by an instance of ModelProjection, which will generally be subclassed in
 *  tandem with Model and ModelFactory.  Model and ModelProjection participate in a
 *  Observer design pattern, with a Model broadcasting changes in its parameters to its
 *  associated ModelProjection objects.
 *
 *  \sa ModelFactory
 *  \sa ModelProjection
 */
class Model : public boost::enable_shared_from_this<Model> {
public:
    typedef boost::shared_ptr<Model> Ptr;
    typedef boost::shared_ptr<Model const> ConstPtr;

    /**
     *  \brief Create a Footprint that would contain a projection of the Model.
     */
    virtual Footprint::Ptr computeProjectionFootprint(
        PsfConstPtr const & psf,
        WcsConstPtr const & wcs
    ) const = 0;

    /**
     *  \brief Create an image-coordinate bounding box that would contain a
     *  projection of the Model.
     */
    virtual lsst::afw::geom::BoxD computeProjectionEnvelope(
        PsfConstPtr const & psf,
        WcsConstPtr const & wcs
    ) const = 0;

    /**
     *  \brief Create an ra/dec bounding ellipse for the Model.
     */
    virtual lsst::afw::geom::ellipses::Ellipse::Ptr computeBoundingEllipse() const = 0;

    /// \brief Return a vector of the linear parameters.
    ParameterVector const & getLinearParameterVector() const { 
        return *_linearParameterVector; 
    }

    /// \brief Return an iterator to the beginning of the linear parameters.
    ParameterConstIterator getLinearParameterIter() const { 
        return _linearParameterVector->data(); 
    }

    /// \brief Return the number of linear parameters.
    int const getLinearParameterSize() const { 
        return _linearParameterVector->size(); 
    }

    /// \brief Set the linear parameters.
    void setLinearParameters(ParameterConstIterator parameterIter);

    /// \brief Return a vector of the nonlinear parameters.
    ParameterVector const & getNonlinearParameterVector() const { 
        return *_nonlinearParameterVector; 
    }

    /// \brief Return an iterator to the beginning of the nonlinear parameters.
    ParameterConstIterator getNonlinearParameterIter() const { 
        return _nonlinearParameterVector->data(); 
    }

    /// \brief Return the number of nonlinear parameters.
    int const getNonlinearParameterSize() const { 
        return _nonlinearParameterVector->size(); 
    }

    /// \brief Set the nonlinear parameters.
    void setNonlinearParameters(ParameterConstIterator parameterIter);

    /**
     *  \brief Create a new Model with the same type and parameters.
     *
     *  Associated ModelProjection objects are not copied or shared;
     *  the new Model will not have any associated ModelProjections.
     */
    virtual Model::Ptr clone() const = 0;

    virtual ~Model() {}

    /** 
     *  \brief Create a ModelProjection object associated with this.
     */
    virtual boost::shared_ptr<ModelProjection> makeProjection(
        PsfConstPtr const & psf,
        WcsConstPtr const & wcs,
        FootprintConstPtr const & footprint
    ) const = 0;

protected:
    typedef boost::weak_ptr<ModelProjection> ProjectionWeakPtr;
    typedef std::list<ProjectionWeakPtr> ProjectionList;

    /// \brief Initialize the Model and allocate space for the parameter vectors.
    Model(int linearParameterSize, int nonlinearParameterSize)       
      : _linearParameterVector(boost::make_shared<ParameterVector>(linearParameterSize)),
        _nonlinearParameterVector(boost::make_shared<ParameterVector>(nonlinearParameterSize)),
        _projectionList()
    {}

    /**
     * \brief Deep-copy the Model.
     *
     * This is a deep copy of the model parameters, projections will not be
     * associated with the new Model
     */ 
    explicit Model(Model const & model) 
      : _linearParameterVector(boost::make_shared<ParameterVector>(model.getLinearParameterVector())),
        _nonlinearParameterVector(boost::make_shared<ParameterVector>(model.getNonlinearParameterVector())),
        _projectionList()
    {}

    /// \brief Notify all associated ModelProjections that the linear parameters have changed.
    void _broadcastLinearParameterChange() const;

    /// \brief Notify all associated ModelProjections that the nonlinear parameters have changed.
    void _broadcastNonlinearParameterChange() const;

    /**
     *  \brief Provide additional code for setLinearParameters().
     *
     *  This will be called by setLinearParameters, after the parameter vector has been updated and
     *  before the call to _broadcastLinearParameterChange().
     */
    virtual void _handleLinearParameterChange() {}

    /**
     *  \brief Provide additional code for setNonlinearParameters().
     *
     *  This will be called by setNonlinearParameters, after the parameter vector has been updated
     *  and before the call to _broadcastNonlinearParameterChange().
     */
    virtual void _handleNonlinearParameterChange() {}

    /// \brief Add a newly-created projection to the list of listeners.
    void _registerProjection(boost::shared_ptr<ModelProjection> const & projection) const;

    boost::shared_ptr<ParameterVector> _linearParameterVector;
    boost::shared_ptr<ParameterVector> _nonlinearParameterVector;

private:
    friend class ModelFactory;

    void operator=(Model const & other) { assert(false); } // Assignment disabled.

    mutable ProjectionList _projectionList;
};

}}} // namespace lsst::meas::multifit

#endif // !LSST_MEAS_MULTIFIT_MODEL_H
