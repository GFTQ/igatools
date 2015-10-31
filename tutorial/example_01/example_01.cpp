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

// [includes]
#include <igatools/basis_functions/bspline.h>
// [includes]
// [include_writer]
#include <igatools/io/writer.h>
// [include_writer]

// [using]
using namespace iga;
using namespace std;
// [using]

int main()
{
  // [dim]
  const int dim = 2;
  // [dim]

  // [grid]
  const int n_knots = 3;
  cout << "Creating a " << dim << " dimensional cartesian grid" << endl;
  auto grid = Grid<dim>::create(n_knots);
  cout << "Number of elements: ";
  cout << grid->get_num_all_elems() << endl;
  // [grid]

  // [plot_grid]
  Writer<dim> output(grid);
  output.save("grid_2D");
  // [plot_grid]

  // [space]
  const int degree = 2;
  cout << "Creating a spline space of degree " << degree << endl;
  auto space = SplineSpace<dim>::create(degree, grid);
  // [space]

  // [basis]
  cout << "Creating the basis for the spline space" << endl;
  auto basis = BSpline<dim>::create(space);
  cout << "Number of basis functions: ";
  cout << basis->get_num_basis() << endl;
  // [basis]

  return 0;
}



