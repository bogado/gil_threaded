/* 
 *  Copyright 2009 Victor Bogado da Silva Lins <victor@tecgraf.puc-rio.br>
 *
 *  Use, modification and distribution are to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */

/** \file
 *  \brief Multithreaded versions of the algorithms defined by gil.
 *
 *  All algorithms defined in here work in the same manner as the 
 *  algorithms with the same name defined by the gil library, with 
 *  the difference that they subdivide the views into 'n' subviews and
 *  operate on each subview simultaneosly in a separated thread.
 *
 *  The algorithms receive an optional parameter that indicates the 
 *  number of threads that the user wants to use. The default is 2, 
 *  witch the minimum number of threads that would make sense using.
 *
 * \author Victor Bogado da Silva Lins
 * \date 2009
 */
#ifndef GIL_THREADED_ALGORITHM_HPP_INCLUDED
#define GIL_THREADED_ALGORITHM_HPP_INCLUDED

#include <boost/gil/algorithm.hpp>
#include <boost/thread.hpp>

#include "algorithm_private.hpp"

namespace boost { namespace gil { namespace threaded {

    template <typename ViewType, typename PixelType>
    void fill_pixels(ViewType view, PixelType pixel, int nt = 2)
    {
        detail::fill_pixels_caller<PixelType> caller(pixel);
        detail::apply_algorithm_fn(caller, view, nt);
    }

    template <typename ViewType, typename FunctionType>
    void for_each_pixel(ViewType view, FunctionType func, int nt = 2)
    {
        detail::for_each_caller<FunctionType> caller(func);
        detail::apply_algorithm_fn(caller, view, nt);
    }

    template <typename SrcView, typename DestView, typename ConverterType>
    void transform_pixels(
        SrcView src, DestView dest, ConverterType conv, int nt = 2)
    {
        detail::transform_pixels_caller<ConverterType> caller(conv);
        detail::apply_algorithm_fn(caller, src, dest, nt);
    }

    template <typename SrcView, typename DestView, typename ConverterType>
    void copy_and_convert_pixels(
        SrcView src, DestView dest, ConverterType conv, int nt = 2)
    {
        detail::copy_and_convert_pixels_caller<ConverterType> caller(conv);
        detail::apply_algorithm_fn(caller, src, dest, nt);
    }

    template <typename SrcView, typename DestView>
    void copy_and_convert_pixels(SrcView src, DestView dest, int nt = 2)
    {

        copy_and_convert_pixels<
            SrcView, 
            DestView, 
            boost::gil::default_color_converter>
                (src, dest, boost::gil::default_color_converter());
    }

} } }

#endif
// vi:expandtab:ts=4 sw=4
