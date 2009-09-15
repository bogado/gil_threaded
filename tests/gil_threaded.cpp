#define BOOST_TEST_MODULE gil_threaded test
#include <boost/test/unit_test.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
#include <boost/gil/extension/threaded/algorithm.hpp>

namespace gil = boost::gil;

BOOST_AUTO_TEST_CASE(fill_pixels)
{
	gil::rgb8_image_t file;
	gil::rgb8_view_t v = gil::view(file);

	gil::fill_pixels(v, gil::rgb8_pixel_t(0,0,0));
	gil::jpeg_read_view("ramses.jpg", v);

//	BOOST_REQUIRE(std::count(v.begin(), v.end(), gil::rgb8_pixel_t(0,0,0)) != v.size());
}
