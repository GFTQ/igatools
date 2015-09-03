//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
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
 *  Test for physical space
 *
 *  author: pauletti
 *  date: Oct 08, 2014
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/functions/function_lib.h>
#include <igatools/functions/identity_function.h>

#include <igatools/basis_functions/phys_space_element_handler.h>
#include <igatools/basis_functions/bspline_element.h>
#include <igatools/basis_functions/physical_space_element.h>
#include <igatools/geometry/push_forward_element.h>


template <int dim, int range=1, int rank=1, int codim = 0>
void cache_init(const ValueFlags flag,
                const int n_knots = 5, const int deg=1)
{
  OUTSTART

  using BspSpace = BSplineSpace<dim, range, rank>;
  using Space    = PhysicalSpace<dim,range,rank,codim, Transformation::h_grad>;
  auto grid      = Grid<dim>::create(n_knots);
  auto ref_space = BspSpace::create(deg, grid);

  using Function = functions::LinearFunction<dim, 0, dim+codim>;
  typename Function::Value    b;
  typename Function::Gradient A;
  for (int i=0; i<Space::space_dim; i++)
  {
    for (int j=0; j<dim; j++)
      if (j == i)
        A[j][j] = 2.;
    b[i] = i;
  }

  auto quad = QGauss<dim>(2);
  auto map_func = Function::create(grid, IdentityFunction<dim>::create(grid), A, b);
  auto space = Space::create(ref_space, map_func);


  auto elem_handler = space->create_cache_handler();
  elem_handler->reset(flag, quad);
  elem_handler->print_info(out);

  OUTEND
}



template <int dim, int range=1, int rank=1, int codim = 0>
void cache_init_elem(const ValueFlags flag,
                     const int n_knots = 5, const int deg=1)
{
//    const int k = dim;
  OUTSTART

  using BspSpace = BSplineSpace<dim, range, rank>;
  using Space    = PhysicalSpace<dim,range,rank,codim, Transformation::h_grad>;

  auto grid  = Grid<dim>::create(n_knots);
  auto ref_space = BspSpace::create(deg, grid);

  using Function = functions::LinearFunction<dim, 0, dim+codim>;
  typename Function::Value    b;
  typename Function::Gradient A;
  for (int i=0; i<Space::space_dim; i++)
  {
    for (int j=0; j<dim; j++)
      if (j == i)
        A[j][j] = 2.;
    b[i] = i;
  }

  auto quad = QGauss<dim>(2);
  auto map_func = Function::create(grid, IdentityFunction<dim>::create(grid), A, b);
  auto space = Space::create(ref_space, map_func);

  auto elem_handler = space->create_cache_handler();
  elem_handler->reset(flag, quad);

  auto elem = space->begin();
  elem_handler->init_element_cache(elem);
  elem->print_cache_info(out);

  OUTEND
}


template <int dim, int range=1, int rank=1, int codim = 0>
void cache_fill_elem(const ValueFlags flag,
                     const int n_knots = 5, const int deg=1)
{
  OUTSTART

//   const int k = dim;
  using BspSpace = BSplineSpace<dim, range, rank>;
  using Space    = PhysicalSpace<dim,range,rank,codim, Transformation::h_grad>;

  auto grid  = Grid<dim>::create(n_knots);
  auto ref_space = BspSpace::create(deg, grid);

  using Function = functions::LinearFunction<dim, 0, dim+codim>;
  typename Function::Value    b;
  typename Function::Gradient A;
  for (int i=0; i<Space::space_dim; i++)
  {
    for (int j=0; j<dim; j++)
      if (j == i)
        A[j][j] = 2.;
    b[i] = i;
  }

  auto quad = QGauss<dim>(2);
  auto map_func = Function::create(grid,IdentityFunction<dim>::create(grid), A, b);
  auto space = Space::create(ref_space, map_func);

  auto elem_handler = space->create_cache_handler();
  elem_handler->reset(flag, quad);

  auto elem = space->begin();
  auto end = space->end();
  elem_handler->init_element_cache(elem);
  for (; elem != end; ++elem)
  {
    elem_handler->fill_element_cache(elem);
    elem->print_cache_info(out);
  }

  OUTEND
}



template <int dim, int range=1, int rank=1, int codim = 0>
void cache_get_elem_values(const ValueFlags flag,
                           const int n_knots = 5, const int deg=1)
{
  OUTSTART
  const int k = dim;
  using BspSpace = BSplineSpace<dim, range, rank>;
  using Space    = PhysicalSpace<dim,range,rank,codim, Transformation::h_grad>;

  auto grid  = Grid<dim>::create(n_knots);
  auto ref_space = BspSpace::create(deg, grid);

  using Function = functions::LinearFunction<dim, 0, dim+codim>;
  typename Function::Value    b;
  typename Function::Gradient A;
  for (int i=0; i<Space::space_dim; i++)
  {
    for (int j=0; j<dim; j++)
      if (j == i)
        A[j][j] = 2.;
    b[i] = i;
  }

  auto quad = QGauss<dim>(2);
  auto map_func = Function::create(grid, IdentityFunction<dim>::create(grid), A, b);
  auto space = Space::create(ref_space, map_func);

  auto elem_handler = space->create_cache_handler();
  elem_handler->reset(flag, quad);

  auto elem = space->begin();
  auto end = space->end();
  elem_handler->init_element_cache(elem);
  for (; elem != end; ++elem)
  {
    elem_handler->fill_element_cache(elem);
    elem->template get_basis<_Value, k>(0,DofProperties::active).print_info(out);
  }

  OUTEND
}



int main()
{
  out.depth_console(10);

  cache_init<1>(ValueFlags::value);
  cache_init_elem<1>(ValueFlags::value);
  cache_fill_elem<1>(ValueFlags::value);
  cache_get_elem_values<1>(ValueFlags::value);

  return  0;
}
