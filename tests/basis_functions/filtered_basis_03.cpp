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

/*
 *  Test for solving on a filtered basis
 *
 *  author: pauletti
 *  date: 2015-03-17
 *
 */

//TODO (pauletti, Mar 22, 2015): this is a development step, should be
// splitted and modified to became several unit testing afterwards
#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/basis_functions/bspline.h>
#include <igatools/basis_functions/bspline_element.h>
#include <igatools/linear_algebra/epetra_solver.h>
#include <igatools/functions/identity_function.h>
#include <igatools/functions/ig_function.h>
#include <igatools/io/writer.h>
#include <igatools/basis_functions/basis_tools.h>

using basis_tools::get_boundary_dofs;

using namespace EpetraTools;

struct DofProp
{
  static const std::string interior;
  static const std::string dirichlet;
  static const std::string neumman;
};

const std::string DofProp::interior = "interior";
const std::string DofProp::dirichlet  = "dirichlet";
const std::string DofProp::neumman  = "neumman";


enum  bc : boundary_id
{
  dir=0, neu
};


template<int dim, int range = 1, int rank = 1>
void filtered_dofs(const int deg = 1, const int n_knots = 3)
{
  OUTSTART
  using RefBasis = ReferenceBasis<dim, range, rank>;
  using Basis = BSpline<dim, range, rank>;

  auto grid = Grid<dim>::create(n_knots);

  const std::set<int> dir_face{0};
  const std::set<int> neu_face{3};
//  grid->set_boundary_id(neu_face, bc::neu);


  auto space = SplineSpace<dim,range,rank>::create(deg,grid);
  auto basis = Basis::create(space);

//  std::set<boundary_id>  dir_ids = {bc::dir};
//  auto dir_dofs = get_boundary_dofs<RefBasis>(basis, dir_ids);
  auto dir_dofs = get_boundary_dofs<RefBasis>(basis, dir_face);


  auto dof_dist = space->get_dof_distribution();
  auto int_dofs = dof_dist->get_interior_dofs();


//  std::set<boundary_id>  neu_ids = {bc::neu};
//  auto neu_dofs = get_boundary_dofs<RefBasis>(basis, neu_ids);
  auto neu_dofs = get_boundary_dofs<RefBasis>(basis, neu_face);
  SafeSTLVector<Index> common(dim*range);
  auto end1 =
    std::set_intersection(neu_dofs.begin(), neu_dofs.end(),
                          dir_dofs.begin(), dir_dofs.end(), common.begin());
  common.resize(end1-common.begin());
  for (auto &id : common)
    neu_dofs.erase(id);

  dof_dist->add_dofs_property(DofProp::interior);
  dof_dist->add_dofs_property(DofProp::dirichlet);
  dof_dist->add_dofs_property(DofProp::neumman);


  dof_dist->set_dof_property_status(DofProp::interior, int_dofs, true);
  dof_dist->set_dof_property_status(DofProp::dirichlet, dir_dofs, true);
  dof_dist->set_dof_property_status(DofProp::neumman, neu_dofs, true);

  auto elem = basis->begin();
  auto end  = basis->end();


  auto matrix = create_matrix(*basis,DofProp::interior,Epetra_SerialComm());
  auto rhs = create_vector(matrix->RangeMap());
  auto solution = create_vector(matrix->DomainMap());


  auto elem_handler = basis->create_cache_handler();

  using Flags = basis_element::Flags;

  auto flag = Flags::value | Flags::gradient | Flags::w_measure;
  elem_handler->set_element_flags(flag);

  auto elem_quad = QGauss<dim>::create(deg+1);
  elem_handler->init_element_cache(elem,elem_quad);

  const int n_qp = elem_quad->get_num_points();
  for (; elem != end; ++elem)
  {
    const int n_basis = elem->get_num_basis(DofProp::interior);

    DenseMatrix loc_mat(n_basis, n_basis);
    loc_mat = 0.0;
    DenseVector loc_rhs(n_basis);
    loc_rhs = 0.0;

    elem_handler->fill_element_cache(elem);
    auto phi = elem->get_element_values(DofProp::interior);
    auto grad_phi  = elem->get_element_gradients(DofProp::interior);
    auto w_meas = elem->get_element_w_measures();

    for (int i = 0; i < n_basis; ++i)
    {
      auto grad_phi_i = grad_phi.get_function_view(i);
      for (int j = 0; j < n_basis; ++j)
      {
        auto grad_phi_j = grad_phi.get_function_view(j);
        for (int qp = 0; qp < n_qp; ++qp)
          loc_mat(i,j) +=
            scalar_product(grad_phi_i[qp], grad_phi_j[qp])
            * w_meas[qp];
      }
      auto phi_i = phi.get_function_view(i);

      for (int qp=0; qp<n_qp; ++qp)
        loc_rhs(i) += phi_i[qp][0] // f=1
                      * w_meas[qp];
    }

    const auto loc_dofs = elem->get_local_to_global(DofProp::interior);
    matrix->add_block(loc_dofs, loc_dofs, loc_mat);
    rhs->add_block(loc_dofs, loc_rhs);
  }

  matrix->FillComplete();
  matrix->print_info(out);
  rhs->print_info(out);

  auto solver = create_solver(*matrix, *solution, *rhs);
  solver->solve();

  solution->print_info(out);

  const int n_plot_points = 4;
  Writer<dim> writer(basis->get_grid(), n_plot_points);
  using IgFunc = IgGridFunction<dim,range>;
  auto solution_function = IgFunc::const_create(basis, *solution, DofProp::interior);
  writer.template add_field(*solution_function, "solution");
  string filename = "poisson_problem-" + to_string(deg) + "-" + to_string(dim) + "d" ;
  writer.save(filename);

  OUTEND
}




int main()
{
  const int dim = 2;
  const int deg = 1;
  const int n_knots = 5;
  filtered_dofs<dim>(deg, n_knots);
  filtered_dofs<dim>(2, n_knots);

  return 0;
}
