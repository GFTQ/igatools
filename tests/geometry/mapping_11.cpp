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

/**
 *  @file
 *  @brief Mapping using a linear Function
 *  @author pauletti
 *  @date 2014-10-23
 */

#include "../tests.h"

#include <igatools/functions/function_lib.h>
#include <igatools/functions/identity_function.h>
#include <igatools/base/quadrature_lib.h>
#include <igatools/functions/function_element.h>
#include <igatools/geometry/domain.h>
#include <igatools/geometry/domain_element.h>

template<int dim, int codim>
void test()
{
  OUTSTART

  const int space_dim = dim+codim;
  using Function = functions::LinearFunction<dim, 0, space_dim>;
  using Mapping   = Domain<dim, codim>;

  typename Function::Value    b;
  typename Function::Gradient A;
  for (int i=0; i<space_dim; i++)
  {
    for (int j=0; j<dim; j++)
      if (j == i)
        A[j][j] = 2.;
    b[i] = i;
  }
  auto grid = Grid<dim>::create(3);
  auto F = Function::create(grid, IdentityFunction<dim>::create(grid), A, b);

  auto flag = ValueFlags::point | ValueFlags::value | ValueFlags::gradient |
              ValueFlags::hessian |ValueFlags::measure|ValueFlags::w_measure;
  auto quad = QGauss<dim>(2);

  Mapping map(F);
  map.reset(flag, quad);

  auto elem = map.begin();
  auto end  = map.end();

  map.init_cache(elem, Topology<dim>());;
  for (; elem != end; ++elem)
  {
    map.fill_cache(elem, Topology<dim>(), 0);
    out << "Points:" << endl;
    elem->get_points().print_info(out);
    out << endl;
    out << "Values:" << endl;
    elem->template get_values<_Value,dim>(0).print_info(out);
    out << endl;
    out << "Gradients:" << endl;
    elem->template get_values<_Gradient,dim>(0).print_info(out);
    out << endl;
    out << "Hessians:" << endl;
    elem->template get_values<_Hessian,dim>(0).print_info(out);
    out << endl;
//        elem->get_measures().print_info(out);
//        out << endl;
//        elem->get_w_measures().print_info(out);
//        out << endl;
  }

  OUTEND
}


int main()
{
  test<2,0>();
  test<3,0>();
  test<2,1>();

  return 0;
}

