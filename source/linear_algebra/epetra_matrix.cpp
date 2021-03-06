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

#include <igatools/linear_algebra/epetra_matrix.h>

IGA_NAMESPACE_OPEN

#ifdef IGATOOLS_USES_TRILINOS

namespace EpetraTools
{
void Matrix::add_block(const SafeSTLVector<Index> &rows_id,
                       const SafeSTLVector<Index> &cols_id,
                       const DenseMatrix &loc_matrix)
{
  const auto n_rows = rows_id.size();
  const auto n_cols = cols_id.size();

  for (int i = 0 ; i < n_rows ; ++i)
  {
    const double *i_row_data =  &(loc_matrix.data()[i*n_cols]);
    SumIntoGlobalValues(rows_id[i], n_cols, i_row_data, cols_id.data());
  }
}



void Matrix::print_info(LogStream &out) const
{
  const auto n_rows = NumGlobalRows();


  out << "-----------------------------" << std::endl;

  out << "Num. rows    = " << NumGlobalRows() << std::endl;
  out << "Num. cols    = " << NumGlobalCols() << std::endl;
  out << "Num. entries = " << NumGlobalNonzeros() << std::endl;
  out << std::endl;
  out << "Row Index        Col Index        Value" << std::endl;

  for (Index local_row = 0 ; local_row < n_rows; ++local_row)
  {
    const auto &graph = Graph();
    const auto &row_map = graph.RowMap();

    const auto global_row = row_map.GID(local_row);

    Index n_entries_row = NumGlobalEntries(global_row);
    SafeSTLVector<Real> values(n_entries_row);
    SafeSTLVector<Index> columns_id(n_entries_row);

    Index nnz = 0;
    ExtractGlobalRowCopy(global_row,n_entries_row,nnz,values.data(),columns_id.data());

    for (Index i = 0 ; i < n_entries_row ; ++i)
      out << global_row << "       "
          << columns_id[i] << "        "
          << values[i] << std::endl;
  }
  out << "-----------------------------" << std::endl;
}



MatrixPtr
create_matrix(const Graph &graph)
{
  return std::make_shared<Matrix>(Epetra_DataAccess::Copy, graph);
}


}

#endif //IGATOOLS_USES_TRILINOS

IGA_NAMESPACE_CLOSE

