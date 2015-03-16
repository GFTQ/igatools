//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------

#ifndef REFERENCE_SPACE_H_
#define REFERENCE_SPACE_H_

#include <igatools/base/config.h>
#include <igatools/base/array_utils.h>
#include <igatools/base/function_element.h>
#include <igatools/utils/cartesian_product_array.h>
#include <igatools/basis_functions/function_space.h>
#include <igatools/geometry/cartesian_grid.h>
#include <igatools/basis_functions/space_element.h>
#include <igatools/basis_functions/spline_space.h>
#include <igatools/basis_functions/dof_distribution.h>
#include <igatools/utils/multi_array_utils.h>

IGA_NAMESPACE_OPEN

class SpaceManager;


template<Transformation,int, int> class PushForward;

template <int, int, int ,int,Transformation> class PhysicalSpace;

template <int, int, int> class ReferenceElement;
template <int,int,int> class ReferenceElementHandler;

template <int, int, int> class BSplineSpace;
template <int, int, int> class NURBSSpace;


template <int,int,int> class DofDistribution;

/**
 *
 * @ingroup containers
 */
template<int dim_, int range_ = 1, int rank_ = 1>
class ReferenceSpace :
    public FunctionSpaceOnGrid<CartesianGrid<dim_>>
{
public:
    static const int dim       = dim_;
    static const int codim     = 0;
    static const int space_dim = dim_;
    static const int range     = range_;
    static const int rank      = rank_;
    static const bool is_physical_space = false;

    /**
     * See documentation in \ref FunctionSpaceOnGrid
     *
     * @see FunctionSpaceOnGrid
     */
    using PushForwardType = PushForward<Transformation::h_grad, dim, codim>;

    using RefSpace = ReferenceSpace<dim_,range_,rank_>;

    using Func = Function<dim, 0, range, rank>;

    template <int order>
    using Derivative = typename Func::template Derivative<order>;
    using Point = typename Func::Point;
    using Value = typename Func::Value;
    using Div   = typename Func::Div;
    using RefPoint = Point;


    using GridSpace = FunctionSpaceOnGrid<CartesianGrid<dim>>;
    using typename GridSpace::GridType;



    /** Type for the element accessor. */
    using ElementAccessor = ReferenceElement<dim,range,rank>;

    /** Type for iterator over the elements.  */
    using ElementIterator = CartesianGridIterator<ElementAccessor>;

    using ElementHandler = ReferenceElementHandler<dim_, range_, rank_>;

    using SpaceData = SplineSpace<dim_,range_,rank_>;

    using Degrees = typename SpaceData::Degrees;
    using Multiplicity = typename SpaceData::Multiplicity;
    using EndBehaviour = typename SpaceData::EndBehaviour;
    using Periodicity = typename SpaceData::Periodicity;

    using KnotsTable = typename SpaceData::KnotsTable;
    using DegreeTable = typename SpaceData::DegreeTable;
    using MultiplicityTable = typename SpaceData::MultiplicityTable;
    using TensorSizeTable = typename SpaceData::TensorSizeTable;
    using PeriodicityTable = typename SpaceData::PeriodicityTable;
    using EndBehaviourTable = typename SpaceData::EndBehaviourTable;

    using BCTable = typename SpaceData::template ComponentContainer<std::array<BoundaryConditionType,UnitElement<dim>::n_faces>>;

    template <class T>
    using ComponentContainer = typename SpaceData::template ComponentContainer<T>;

    using ComponentMap = typename SpaceData::template ComponentContainer<int>::ComponentMap;

    static const auto n_components = SpaceData::n_components;

protected:

    ReferenceSpace() = delete;


    explicit ReferenceSpace(
        const std::shared_ptr<CartesianGrid<dim_>> grid,
        const std::shared_ptr<DofDistribution<dim_,range_,rank_>> dof_distribution);

public:
    virtual ~ReferenceSpace() = default;

    /**
     * Create and element (defined on this space) with a given flat_index
     */
    virtual std::shared_ptr<ElementAccessor>
    create_element(const Index flat_index) const = 0;

    template <int k>
    using InterGridMap = typename GridType::template InterGridMap<k>;

    template <int k>
    using InterSpaceMap = vector<Index>;

    template <int k>
    using SubRefSpace = ReferenceSpace<k, range, rank>;

    template <int k>
    using SubSpace = PhysicalSpace<k,range,rank, dim-k, Transformation::h_grad>;

    virtual bool is_bspline() const = 0;

    /**
     * Returns the degree of the BSpline space for each component and for each coordinate direction.
     * \return The degree of the BSpline space for each component and for each coordinate direction.
     * The first index of the returned object is the component id, the second index is the direction id.
     */
    virtual const DegreeTable &get_degree() const = 0;

    /** @name Functions for retrieving information about the number of basis function. */
    ///@{

    const TensorSizeTable &get_num_basis_table() const
    {
        return dof_distribution_->get_num_dofs_table();
    }

    Size get_num_basis() const
    {
        return dof_distribution_->get_num_dofs_table().total_dimension();
    }


    Size get_num_basis(const int comp) const
    {
        return dof_distribution_->get_num_dofs_table().get_component_size(comp);
    }

    Size get_num_basis(const int comp, const int dir) const
    {
        return dof_distribution_->get_num_dofs_table()[comp][dir];
    }

    ComponentContainer<Size> get_basis_offset() const
    {
        return dof_distribution_->get_num_dofs_table().get_offset();
    }
    ///@}


    /**
     * Adds an @p offset to the values of the dof ids.
     */
    void add_dofs_offset(const Index offset);

    /**
     * This function returns the global dof id corresponding to the basis function
     * with tensor index <p>tensor_index</p> on the @p comp component of the space.
     */
    Index
    get_global_dof_id(const TensorIndex<dim> &tensor_index,
                      const Index comp) const;

    // TODO (pauletti, Dec 12, 2014): boundary condition is not a general property
    // of the space, rather specific flag for some application, this should be
    // done in some other layer
    /**
     * @name Dealing with the boundary conditions.
     * @return
     */
    ///@{
    /**
     * Returns a const-reference to the table containing
     * the boundary conditions on each face of each scalar component of the space.
     *
     * For example, with the code
     * @code{.cpp}
       const auto &bc_table = space.get_boundary_conditions_table();

       BoundaryConditionType bc_id = bc_table[1][3]; // boundary condition on face 3 of space's component 1
       @endcode
     * we copy to the variable <tt>bc_id</tt> the value of the boundary condition
     * on the face 3 of the space component 1.
     *
     * @sa BoundaryConditionType
     */
    const BCTable &get_boundary_conditions_table() const
    {
        return boundary_conditions_table_;
    }


    /**
     * Returns a reference to the table containing
     * the boundary conditions on each face of each scalar component of the space.
     *
     * For example, with the code
     * @code{.cpp}
       const auto &bc_table = space.get_boundary_conditions_table();

       BoundaryConditionType bc_id = bc_table[1][3]; // boundary condition on face 3 of space's component 1
       @endcode
     * we copy to the variable <tt>bc_id</tt> the value of the boundary condition
     * on the face 3 of the space component 1.
     *
     * @sa BoundaryConditionType
     */
    BCTable &get_boundary_conditions_table()
    {
        return boundary_conditions_table_;
    }
    ///@}

#if 0
    /**
     * Returns a reference to the end behaviour table of the BSpline space.
     */
    virtual EndBehaviourTable &get_end_behaviour_table() = 0;
#endif
    /**
     * Returns a const reference to the end behaviour table of the BSpline space.
     */
    virtual const EndBehaviourTable &get_end_behaviour_table() const = 0;


    virtual const PeriodicityTable &get_periodicity() const = 0;


    std::shared_ptr<const DofDistribution<dim, range, rank> >
    get_dof_distribution() const;

    std::shared_ptr<DofDistribution<dim, range, rank> >
    get_dof_distribution();

#if 0
    // TODO (pauletti, Oct 16, 2014): need to be documented or deleted, check!
    vector<Index> get_loc_to_global(const CartesianGridElement<dim> &element,
                                    const std::string &dofs_property) const;

    vector<Index> get_loc_to_patch(const CartesianGridElement<dim> &element,
                                   const std::string &dofs_property) const;
#endif

    virtual void get_element_dofs(
        const CartesianGridElement<dim> &element,
        vector<Index> &dofs_global,
        vector<Index> &dofs_local_to_patch,
        vector<Index> &dofs_local_to_elem,
        const std::string &dofs_property = DofProperties::none) const = 0;


    std::shared_ptr<SpaceManager> get_space_manager();
    std::shared_ptr<const SpaceManager> get_space_manager() const;


    /** @name Functions involving the element iterator */
    ///@{
    /**
     * Returns a element iterator to the first element of the patch
     * with the property @p element_property.
     */
    ElementIterator begin(const std::string &element_property = ElementProperties::none) const;

    /**
     * Returns a element iterator to the last element of the patch
     * with the property @p element_property.
     */
    ElementIterator last(const std::string &element_property = ElementProperties::none) const;


    /**
     * Returns a element iterator to one-pass the end of patch.
     * with the property @p element_property.
     */
    ElementIterator end(const std::string &element_property = ElementProperties::none) const;
    ///@}

    template<int k>
    std::shared_ptr< SubRefSpace<k> >
    get_ref_sub_space(const int s_id,
                      InterSpaceMap<k> &dof_map,
                      std::shared_ptr<CartesianGrid<k>> sub_grid = nullptr) const;

    template<int k>
    std::shared_ptr<SubSpace<k> >
    get_sub_space(const int s_id, InterSpaceMap<k> &dof_map,
                  std::shared_ptr<CartesianGrid<k>> sub_grid,
                  std::shared_ptr<InterGridMap<k>> elem_map) const;

    virtual void print_info(LogStream &out) const = 0;

    virtual std::shared_ptr<ElementHandler> create_elem_handler() const = 0;

protected:


    /**
     * Container with the local to global basis indices
     * @note The concept of global indices refers to a global numbering of the
     * dofs of all the spaces.
     */
    std::shared_ptr<DofDistribution<dim,range,rank> > dof_distribution_;

    // TODO (pauletti, Dec 12, 2014): boundary condition is not a general property
    // of the space, rather specific flag for some application, this should be
    // done in some other layer
    /**
     * Boundary conditions on each face of each scalar component of the space.
     */
    BCTable boundary_conditions_table_;

};

IGA_NAMESPACE_CLOSE

#endif // #ifndef REFERENCE_SPACE_H_
