#ifndef GIL_THREADED_ALGORITHM_HPP_INCLUDED
#define GIL_THREADED_ALGORITHM_HPP_INCLUDED

#include <boost/gil/algorithm.hpp>
#include <boost/thread.hpp>

namespace boost { namespace gil { namespace threaded {
	namespace detail
	{
		template <typename Algorithm, typename SrcView, typename DestView>
		void apply_algorithm_fn(Algorithm algorithm, SrcView src, int nt)
		{
			typedef typename SrcView::point_t point_t;

			const point_t dim = src.dimensions();
			point_t size = dim;
			point_t start;

			size[point_t::num_dimensions - 1] /= nt;
			start[point_t::num_dimensions - 1] = size[point_t::num_dimensions - 1];

			boost::thread_group group;
			for (int i = 0; i < nt; i++)
			{
				group.add_thread(new boost::thread( 
							algorithm, 
							boost::gil::subimage_view(src, start * i, size)));
			}

			group.join_all();
		}

		template <typename Algorithm, typename SrcView, typename DestView>
		void apply_algorithm_fn(Algorithm algorithm, SrcView src, DestView dst, int nt)
		{
			typedef typename SrcView::point_t point_t;

			const point_t dim = src.dimensions();
			point_t size = dim;
			point_t start;

			size[point_t::num_dimensions - 1] /= nt;
			start[point_t::num_dimensions - 1] = size[point_t::num_dimensions - 1];

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
				boost::gil::fill_pixels(src, pixel)
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
			copy_and_convert_pixels_caller(ConverterType conv) : converter(conv)
			{}

			template <typename SrcView, typename DestView>
			void operator() (SrcView src, DestView dest)
			{
				boost::gil::copy_and_convert_pixels(src, dest, converter);
			}

		private:
			ConverterType converter;
		};
	}

	template <typename ViewType, typename PixelType>
	void fill_pixels(ViewType view, typename PixelType pixel, int nt = 2)
	{
		detail::fill_pixel_caller caller(pixel);
		detail::apply_algorithm_fn(fill_caller, view, nt);
	}

	template <typename ViewType, typename FunctionType>
	void for_each_pixel(ViewType view, FunctionType func, int nt = 2)
	{
		detail::for_each_caller<FunctionType> caller(func);
		detail::apply_algorithm_fn(caller, view, nt);
	}

	template <typename SrcView, typename DestView, typename ConverterType>
	void transform_pixels(SrcView src, DestView dest, ConverterType conv, int nt = 2)
	{
		detail::transform_pixels_caller<ConverterType> caller(conv);
		detail::apply_algorithm_fn(caller, src, dest, nt);
	}

	template <typename SrcView, typename DestView, typename ConverterType>
	void copy_and_convert_pixels(SrcView src, DestView dest, ConverterType conv, int nt = 2)
	{
		detail::copy_and_convert_pixels_caller<ConverterType> caller(conv);
		detail::apply_algorithm_fn(caller, src, dest, nt);
	}

	template <typename SrcView, typename DestView>
	void copy_and_convert_pixels(SrcView src, DestView dest, int nt = 2)
	{
		copy_and_convert_pixels<SrcView, DestView, boost::gil::default_color_converter>(
				src, dest,
				boost::gil::default_color_converter());
	}

} } }

#endif
