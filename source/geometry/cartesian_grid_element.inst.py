#-+--------------------------------------------------------------------
# Igatools a general purpose Isogeometric analysis library.
# Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
#
# This file is part of the igatools library.
#
# The igatools library is free software: you can use it, redistribute
# it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-+--------------------------------------------------------------------

from init_instantiation_data import *

include_files = ['../../source/geometry/cartesian_grid_iterator.cpp']
data = Instantiation(include_files)
(f, inst) = (data.file_output, data.inst)



sub_dim_members = [
             'Real CartesianGridElement<dim>::get_measure<k>(const int j) const;',
             'ValueVector<Real> CartesianGridElement<dim>::get_w_measures<k>(const int j) const;',
             'const typename CartesianGridElement<dim>::Point CartesianGridElement<dim>::get_coordinate_lengths<k>(const int j) const;',
             'ValueVector<typename CartesianGridElement<dim>::Point> CartesianGridElement<dim>::get_points<k>(const int j) const;',
             'bool CartesianGridElement<dim>::is_boundary<k>(const Index sub_elem_id) const;',
             'bool CartesianGridElement<dim>::is_boundary<k>() const;']

elems = []

for dim in inst.sub_domain_dims:
    acc = 'CartesianGridElement<%d>' %(dim) 
    f.write('template class %s; \n' %(acc))
    elems.append(acc)
    for it in inst.iterators:
        iterator = it.replace('Accessor','%s' % (acc) )
        f.write('template class %s; \n' %iterator)
    for fun in sub_dim_members:
        k = dim
        s = fun.replace('k', '%d' % (k)).replace('dim', '%d' % (dim));
        f.write('template ' + s + '\n')


for dim in inst.domain_dims:
    acc = 'CartesianGridElement<%d>' %(dim) 
    f.write('template class %s; \n' %(acc))
    elems.append(acc)
    for it in inst.iterators:
        iterator = it.replace('Accessor','%s' % (acc) )
        f.write('template class %s; \n' %iterator)
    for fun in sub_dim_members:
        for k in inst.sub_dims(dim):
            s = fun.replace('k', '%d' % (k)).replace('dim', '%d' % (dim));
            f.write('template ' + s + '\n')
            


#---------------------------------------------------
f.write('IGA_NAMESPACE_CLOSE\n')

f.write('#ifdef SERIALIZATION\n')
id = 0 
for elem in unique(elems):
    alias = 'CartesianGridElementAlias%d' %(id)
    f.write('using %s = iga::%s; \n' % (alias, elem))
    f.write('BOOST_CLASS_EXPORT_IMPLEMENT(%s) \n' %alias)
    f.write('template void %s::serialize(OArchive &, const unsigned int);\n' % alias)
    f.write('template void %s::serialize(IArchive &, const unsigned int);\n' % alias)
    id += 1 
f.write('#endif // SERIALIZATION\n')
    
f.write('IGA_NAMESPACE_OPEN\n')
#---------------------------------------------------
