#include <iostream>
#include <algorithm>

#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
#include <boost/gil/extension/threaded/algorithm.hpp>

namespace gil = boost::gil;

gil::rgb8_image_t load_image()
{
    gil::rgb8_image_t image;

    gil::rgb8_view_t v = gil::view(image);
    gil::fill_pixels(v, gil::rgb8_pixel_t(0,0,0));

    gil::jpeg_read_image("ramses.jpg", image);
    v = gil::view(image);

    BOOST_REQUIRE_NE(std::count(v.begin(), v.end(), gil::rgb8_pixel_t(0,0,0)), v.size());

    return image;
}

BOOST_AUTO_TEST_CASE(fill)
{
    gil::rgb8_image_t image = load_image();
    gil::rgb8_view_t v = gil::view(image);

    gil::threaded::fill_pixels(v, gil::rgb8_pixel_t(0,128,255));
    
    BOOST_CHECK_EQUAL(std::count(v.begin(), v.end(), gil::rgb8_pixel_t(0,128,255)), v.size());
}

struct count_red
{
    size_t val;

    count_red() : val(0)
    {}

    void operator()(gil::rgb8_pixel_t pix)
    {
        if (gil::at_c<0>(pix) > 128 && gil::at_c<1>(pix) < 128 && gil::at_c<2>(pix) < 128)
            val++;
    }
};

BOOST_AUTO_TEST_CASE(for_each_pixel)
{
    // image to be processed with the usual method.
    gil::rgb8_image_t image1 = load_image();
    gil::rgb8_view_t v1 = gil::view(image1);
    count_red c1;

    // image to be processed with multithreads.
    gil::rgb8_image_t image2 = load_image();
    gil::rgb8_view_t v2 = gil::view(image2);
    count_red c2;

    gil::for_each_pixel          (v1, c1);
    gil::threaded::for_each_pixel(v2, c2);

    BOOST_WARN_EQUAL (c1.val, 0);
    BOOST_CHECK_EQUAL(c1.val, c2.val);
}

gil::rgb8_pixel_t invert_pixel(const gil::rgb8_pixel_t& orig)
{
    return gil::rgb8_pixel_t(
            gil::at_c<0>(orig) ^ 0xff,
            gil::at_c<1>(orig) ^ 0xff,
            gil::at_c<2>(orig) ^ 0xff);
}

gil::rgb8_pixel_t op(const gil::rgb8_pixel_t& p1, const gil::rgb8_pixel_t& p2)
{
    return  gil::rgb8_pixel_t(
            (gil::at_c<0>(p1) << 1) + (gil::at_c<0>(p2) >> 1),
            (gil::at_c<1>(p1) << 1) + (gil::at_c<1>(p2) >> 1),
            (gil::at_c<2>(p1) << 1) + (gil::at_c<2>(p2) >> 1));
}

BOOST_AUTO_TEST_CASE(transform_pixels)
{
    gil::rgb8_image_t image1 = load_image();
    gil::rgb8_view_t orig    = gil::view(image1);

    gil::rgb8_image_t dest1(orig.dimensions());
    gil::rgb8_image_t dest2(orig.dimensions());

    gil::rgb8_view_t v1 = gil::view(dest1);
    gil::rgb8_view_t v2 = gil::view(dest2);

    gil::transform_pixels          (orig, v1, &invert_pixel);
    gil::threaded::transform_pixels(orig, v2, &invert_pixel);

    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v2.begin()));

    gil::rgb8_image_t dest3(orig.dimensions());

    gil::rgb8_view_t v3 = gil::view(dest3);

    gil::transform_pixels          (orig, v1, v2, &op);
    gil::threaded::transform_pixels(orig, v1, v3, &op);

    BOOST_CHECK(std::equal(v2.begin(), v2.end(), v3.begin()));
}

void red(const gil::rgb8_pixel_t& src, gil::gray8_pixel_t& dst)
{
    dst = gil::at_c<0>(src);
}

BOOST_AUTO_TEST_CASE(copy_and_convert_pixels)
{
    gil::rgb8_image_t image1 = load_image();
    gil::rgb8_view_t orig    = gil::view(image1);

    gil::gray8_image_t dest1(orig.dimensions());
    gil::gray8_image_t dest2(orig.dimensions());

    gil::gray8_view_t v1 = gil::view(dest1);
    gil::gray8_view_t v2 = gil::view(dest2);

    gil::copy_and_convert_pixels          (orig, v1);
    gil::threaded::copy_and_convert_pixels(orig, v2);

    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v2.begin()));

    gil::copy_and_convert_pixels          (orig, v1, red);
    BOOST_WARN(std::equal(v1.begin(), v1.end(), v2.begin()));
    gil::threaded::copy_and_convert_pixels(orig, v2, red);

    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v2.begin()));
}

struct generator
{
    generator() : start(0)
    {}

    gil::rgb8_pixel_t operator() ()
    {
        gil::rgb8_pixel_t ret = gil::rgb8_pixel_t(start, 0, 255 - start);
        start++;
        return ret;
    }

private:
    unsigned char start;
};

BOOST_AUTO_TEST_CASE(generate)
{
    gil::gray8_image_t dest1(256,256);
    gil::gray8_image_t dest2(dest1.dimensions());

    gil::gray8_view_t v1 = gil::view(dest1);
    gil::gray8_view_t v2 = gil::view(dest2);

    gil::generate_pixels          (v1, generator());
    gil::threaded::generate_pixels(v2, generator());

    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v2.begin()));
}

// vi:expandtab:ts=4 sw=4
