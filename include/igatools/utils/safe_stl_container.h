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

#ifndef SAFE_STL_CONTAINER_H_
#define SAFE_STL_CONTAINER_H_

#include <igatools/base/config.h>
#include <igatools/base/print_info_utils.h>


IGA_NAMESPACE_OPEN

/**
 * @brief Class used to provide
 * bounds checking (in Debug mode) to an STL container (as std::vector or std::array).
 *
 * @tparam STLContainer std::vector or std::array
 *
 * @author M. Martinelli, 2015
 *
 * @ingroup serializable
 */
template <class STLContainer>
class SafeSTLContainer : public STLContainer
{
public:
  /** Inherit the constructors of the base class. */
  using STLContainer::STLContainer;

  /**
   * Returns the number of entries in the container.
   */
  Size size() const noexcept
  {
    return STLContainer::size();
  }



  /**
   * @name Printing info
   */
  ///@{
private:

  template <class T = STLContainer>
  EnableIf<!std::is_pointer<T>::value, const T &>
  get_ref_base_container() const
  {
    return (*this);
  }

  template <class T = STLContainer>
  EnableIf<!std::is_pointer<T>::value, const typename std::remove_pointer<T>::type &>
  get_ref_base_container() const
  {
    return *(*this);
  }

  template <class A>
  EnableIf<has_print_info<A>(0), void>
  t_print_info(LogStream &out) const
  {
    int entry_id = 0;
    for (auto &entry : *this)
    {
      out.begin_item("Entry id: " + std::to_string(entry_id++));
      entry.print_info(out);
      out.end_item();
    }
  }

  template <class A>
  EnableIf<(!has_print_info<A>(0)), void>
  t_print_info(LogStream &out) const
  {
    out << "[ ";
    for (auto &entry : *this)
      out << entry << " ";
    out << "]";
  }

public:

  /**
   * Prints the content of the vector on the LogStream @p out.
   * Its use is intended mainly for testing and debugging purpose.
   */
  void print_info(LogStream &out) const
  {
    t_print_info<typename STLContainer::value_type>(out);
  }
  ///@}
};


IGA_NAMESPACE_CLOSE


#endif // SAFE_STL_CONTAINER_H_
