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
 *  @brief Developing domain class
 *  @author pauletti
 *  @date 2015
 */
#include <igatools/base/quadrature_lib.h>
#include <igatools/geometry/domain.h>
#include <igatools/geometry/formula_domain.h>
#include <igatools/geometry/domain_lib.h>

#include "../tests.h"


template<int dim, int codim>
void domain()
{
  OUTSTART

  using Grid = Grid<dim>;
  //using Domain = Domain<dim, codim>;

  using Domain = domains::BallDomain<dim>;
  auto grid = Grid::const_create();
  auto domain = Domain::create(grid);

  using Flags = typename Domain::ElementAccessor::Flags;

  auto flag = Flags::w_measure | Flags::point;
  //auto s_flag = Flags::point;
  auto handler = domain->create_cache_handler();

  handler->template set_flags<dim>(flag);
  //handler->template set_flags<sdim>(s_flag);

  auto quad   = QGauss<dim>::create(2);
//    auto s_quad = QGauss<sdim>::create(1);

  auto elem = domain->cbegin();
  handler->init_cache(elem, quad);
//    cache_handler->template init_cache<sdim>(elem, s_quad);

  for (; elem != domain->cend(); ++elem)
  {
    handler->template fill_cache<dim>(elem, 0);
    elem->template get_points<dim>(0).print_info(out);
    elem->template get_w_measures<dim>(0).print_info(out);
    out << endl;

//      for (auto &s_id : UnitElement<dim>::template elems_ids<sdim>())
//      {
//       handler->template fill_cache<sdim>(elem, s_id);
//        elem->template get_points<sdim>(s_id).print_info(out);
//        out << endl;
//      }
//      out << endl;
  }

  OUTEND
}


int main()
{
  //domain<0,0>();
  domain<1,0>();
  domain<2,0>();
  domain<3,0>();
  //domain<2,1>();

  return 0;
}

