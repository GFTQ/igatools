//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
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

#ifndef REFERENCE_ELEMENT_H_
#define REFERENCE_ELEMENT_H_

#include <igatools/base/config.h>
#include <igatools/basis_functions/basis_element.h>
#include <igatools/basis_functions/reference_basis_handler.h>

IGA_NAMESPACE_OPEN

template <int, int, int> class ReferenceBasis;

/**
 *
 * @ingroup elements
 */
template <int dim, int range, int rank>
class ReferenceBasisElement : public BasisElement<dim,0,range,rank>
{
public:

  /** Type required by the GridForwardIterator templated iterator */
  using ContainerType = const ReferenceBasis<dim,range,rank> ;

  using Basis = ReferenceBasis<dim,range,rank>;
  using ConstBasis = const ReferenceBasis<dim,range,rank>;

  using parent_t = BasisElement<dim,0,range,rank>;

  using RefPoint = typename Basis::RefPoint;
  using Point = typename Basis::Point;
  using Value = typename Basis::Value;

  template <int order>
  using Derivative = typename Basis::template Derivative<order>;

  using Div = typename Basis::Div;


  using GridType = Grid<dim>;
  using IndexType = typename GridType::IndexType;
  using List = typename GridType::List;
  using ListIt = typename GridType::ListIt;

  using GridElem = GridElement<dim>;

public:
  ReferenceBasisElement() = delete;

  /**
   * Copy constructor. Not allowed to be used.
   */
  ReferenceBasisElement(const ReferenceBasisElement<dim,range,rank> &elem) = delete;

  /**
   * Constructs an accessor to element number index of a
   * ReferenceBasis basis.
   */
  ReferenceBasisElement(const std::shared_ptr<ConstBasis> &basis);


  virtual ~ReferenceBasisElement() = default;

  /**
   * Returns the <tt>k</tt> dimensional j-th sub-element measure
   * multiplied by the weights of the quadrature on the unit element.
   */
  template <int sdim>
  ValueVector<Real> get_w_measures(const int s_id) const;

  /**
   * Returns the gradient determinant of the identity map at the dilated quadrature points.
   */
  ValueVector<Real> get_element_w_measures() const;


//    using OffsetTable = typename Basis::template ComponentContainer<int>;
  using OffsetTable = SafeSTLArray<int,Basis::n_components+1>;

  using TensorSizeTable = typename Basis::SpSpace::TensorSizeTable;

protected:

  /** Number of scalar basis functions along each direction, for all basis components. */
  TensorSizeTable n_basis_direction_;

  /**
   * Offset of the scalar basis functions across the different components.
   *
   * @note The last entry of the array contains the total number of scalar basis functions.
   */
  OffsetTable comp_offset_;

  using Indexer = CartesianProductIndexer<dim>;
  using IndexerPtr = std::shared_ptr<Indexer>;
  using IndexerPtrTable = typename Basis::template ComponentContainer<IndexerPtr>;

  /** Hash table for fast conversion between flat-to-tensor basis function ids. */
  IndexerPtrTable basis_functions_indexer_;

  std::shared_ptr<const Basis> basis_;

public:
  using parent_t::get_num_basis;


  /**
   * Returns the basis function ID offset between the different components.
   */
  OffsetTable get_basis_offset() const;

  /**
   * Number of non-zero scalar basis functions associated
   * with the i-th basis component on the element.
   * This makes sense as a reference B-spline basis
   * is only allowed to be of the cartesian product type
   * V = V1 x V2 x ... X Vn.
   */
  int get_num_basis_comp(const int i) const;

  virtual void print_info(LogStream &out) const override;
};

IGA_NAMESPACE_CLOSE

#endif // #ifndef REFERENCE_ELEMENT_H_
