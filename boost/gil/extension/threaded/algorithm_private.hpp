/* 
 *  Copyright 2009 Victor Bogado da Silva Lins <victor@tecgraf.puc-rio.br>
 *
 *  Use, modification and distribution are to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */

/** \file
 *  \brief  Internal support for the threaded algorithms.
 *  \author Victor Bogado da Silva Lins
 *  \date   2009
 */

#ifndef GIL_THREADED_ALGORITHM_PRIVATE_HPP_INCLUDED
#define GIL_THREADED_ALGORITHM_PRIVATE_HPP_INCLUDED

#include <boost/tuple/tuple.hpp>

namespace boost { namespace gil { namespace threaded { namespace detail {

/// Turn all views in the tuples into identical subviews.
template <typename Point>
inline void subimage(const tuples::null_type&, Point, Point)
{}

template <typename ViewTuple, typename Point>
void subimage(ViewTuple& views, Point start, Point size)
{
    views.get_head() = subimage_view(views.get_head(), start, size);
    subimage(views.get_tail(), start, size);
}

template <typename Algorithm, typename Viewtuple>
void apply_algorithm_fn(Algorithm algorithm, Viewtuple views, int nt)
{
    typedef typename Viewtuple::head_type view_src_t;
    typedef typename view_src_t::point_t point_t;

    view_src_t src = views.get<0>();
    const point_t dim = src.dimensions();
    point_t size = dim;
    point_t start;

    size[point_t::num_dimensions - 1] /= nt;
    start[point_t::num_dimensions - 1] = 
        size[point_t::num_dimensions - 1];

    thread_group group;
    for (int i = 0; i < nt; i++)
    {
        Viewtuple subviews = views;
        subimage(subviews, start * i, size);
        group.add_thread(new thread(algorithm, subviews));
    }

    group.join_all();
}

template <typename PixelType>
struct fill_pixels_caller
{
    fill_pixels_caller(PixelType pix) : pixel(pix)
    {}

    template <typename ViewType>
    void operator() (tuple<ViewType> views)
    {
        boost::gil::fill_pixels(get<0>(views), pixel);
    }

private:
    PixelType pixel;
};

template <typename ConverterType>
struct for_each_caller
{
    for_each_caller(ConverterType conv) : converter(conv)
    {}

    template <typename SrcView>
    void operator() (tuple<SrcView> views)
    {
        boost::gil::for_each_pixel(get<0>(views), converter);
    }

private:
    ConverterType converter;
};

template <typename ConverterType>
struct transform_pixels_caller
{
    transform_pixels_caller(ConverterType conv) : converter(conv)
    {}

    template <typename SrcView, typename DestView>
    void operator() (tuple<SrcView, DestView> views)
    {
        boost::gil::transform_pixels(get<0>(views), get<1>(views), converter);
    }

private:
    ConverterType converter;
};

template <typename ConverterType>
struct copy_and_convert_pixels_caller
{
    copy_and_convert_pixels_caller(ConverterType conv) :
        converter(conv)
    {}

    template <typename SrcView, typename DestView>
    void operator() (tuple<SrcView, DestView> views)
    {
        boost::gil::copy_and_convert_pixels(get<0>(views), get<1>(views), converter);
    }

private:
    ConverterType converter;
};

} } } }

#endif
// vi:expandtab:ts=4 sw=4
