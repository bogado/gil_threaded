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

namespace boost { namespace gil { namespace threaded { namespace detail {

template <typename Algorithm, typename SrcView, typename DestView>
void apply_algorithm_fn(Algorithm algorithm, SrcView src, int nt)
{
    typedef typename SrcView::point_t point_t;

    const point_t dim = src.dimensions();
    point_t size = dim;
    point_t start;

    size[point_t::num_dimensions - 1] /= nt;
    start[point_t::num_dimensions - 1] = 
        size[point_t::num_dimensions - 1];

    boost::thread_group group;
    for (int i = 0; i < nt; i++)
    {
        group.add_thread(new boost::thread( 
                    algorithm, 
                    boost::gil::subimage_view(
                        src, start * i, size)));
    }

    group.join_all();
}

template <typename Algorithm, typename SrcView, typename DestView>
void apply_algorithm_fn(Algorithm algorithm,
    SrcView src, DestView dst, int nt)
{
    typedef typename SrcView::point_t point_t;

    const point_t dim = src.dimensions();
    const int last_dimension = point_t::num_dimensions - 1;
    point_t size = dim;
    // hopefully default constructor initiates on the origin
    point_t start; 

    size[last_dimension] /= nt;
    start[last_dimension] = size[last_dimension];

    boost::thread_group group;
    for (int i = 0; i < nt; i++)
    {
        group.add_thread( new boost::thread( 
            algorithm, 
            boost::gil::subimage_view(src, start * i, size),
            boost::gil::subimage_view(dst, start * i, size)));
    }

    group.join_all();
}

template <typename PixelType>
struct fill_pixels_caller
{
    fill_pixels_caller(PixelType pix) : pixel(pix)
    {}

    template <typename ViewType>
    void operator() (ViewType src)
    {
        boost::gil::fill_pixels(src, pixel);
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
    void operator() (SrcView src)
    {
        boost::gil::for_each_pixel(src, converter);
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
    void operator() (SrcView src, DestView dest)
    {
        boost::gil::transform_pixels(src, dest, converter);
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
    void operator() (SrcView src, DestView dest)
    {
        boost::gil::copy_and_convert_pixels(src, dest, converter);
    }

private:
    ConverterType converter;
};

} } } }

#endif
// vi:expandtab:ts=4 sw=4
