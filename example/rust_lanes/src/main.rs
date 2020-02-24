use opencv::{
    core::{self, Point, Scalar, Vec3b},
    highgui, imgcodecs, imgproc,
    prelude::*,
    types,
};

use std::{env, process};

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("usage");
        eprintln!("cargo run ../../img/img.jpg");
        process::exit(1);
    }
    let img_original   = imgcodecs::imread(&args[1], imgcodecs::IMREAD_COLOR).unwrap();
    let mut img_gray   = core::UMat::new(core::UMatUsageFlags::USAGE_DEFAULT).unwrap();
    let mut img_gauss  = core::UMat::new(core::UMatUsageFlags::USAGE_DEFAULT).unwrap();
    let mut img_canny  = core::UMat::new(core::UMatUsageFlags::USAGE_DEFAULT).unwrap();
    let mut img_result = core::UMat::new(core::UMatUsageFlags::USAGE_DEFAULT).unwrap();
    img_original.copy_to(&mut img_result).unwrap();

    //  1. Convert to gray
    imgproc::cvt_color(&img_original, &mut img_gray, imgproc::COLOR_RGB2GRAY, 0).unwrap();

    // 2. Convert to gaussian
    let ksize = core::Size::new(5, 5);
    let sigma_x = 0.0_f64;
    let sigma_y = 0.0_f64;
    imgproc::gaussian_blur(
        &img_gray,
        &mut img_gauss,
        ksize,
        sigma_x,
        sigma_y,
        core::BORDER_DEFAULT,
    )
    .unwrap();

    // 3. Canny
    let min_thresh = 128_f64;
    let max_thresh = 130_f64;
    let aperture = 3_i32;
    imgproc::canny(
        &img_gauss,
        &mut img_canny,
        min_thresh,
        max_thresh,
        aperture,
        true,
    )
    .unwrap();

    // 4 Region of Interest
    let x_size = img_canny.cols().unwrap();
    let y_size = img_canny.rows().unwrap();
    let x_a = (x_size as f32 * 0.50) as i32;
    let x_b = (x_size as f32 * 0.05) as i32;
    let y_a = (y_size as f32 * 0.62) as i32;

    let mut mask = Mat::new_rows_cols_with_default(
        y_size,
        x_size,
        img_canny.channels().unwrap(),
        Scalar::new(0.0, 0.0, 0.0, 0.),
    )
    .unwrap();
    let mut ps = types::VectorOfMat::new();
    let mut p1 = unsafe { Mat::new_rows_cols(4, 2, i32::typ()) }.unwrap();
    p1.at_row_mut::<i32>(0)
        .unwrap()
        .copy_from_slice(&[0, y_size]);
    p1.at_row_mut::<i32>(1)
        .unwrap()
        .copy_from_slice(&[(x_a - x_b), y_a]);
    p1.at_row_mut::<i32>(2)
        .unwrap()
        .copy_from_slice(&[(x_a + x_b), y_a]);
    p1.at_row_mut::<i32>(3)
        .unwrap()
        .copy_from_slice(&[x_size, y_size]);
    ps.push(p1);
    imgproc::fill_poly(
        &mut mask,
        &ps,
        Scalar::new(255., 255., 255., 1.),
        imgproc::LINE_8,
        0,
        Point::default(),
    )
    .unwrap();
    let mut masked_edges =
        Mat::new_rows_cols_with_default(y_size, x_size, Vec3b::typ(), Scalar::default()).unwrap();
    core::bitwise_and(&img_canny, &img_canny, &mut masked_edges, &mask).unwrap();

    // 5. Hough Lines
    let rho       = 1.0_f64;
    let theta     = std::f64::consts::PI / 180.0_f64;
    let threshold = 55_i32;
    let min_theta = 50.0_f64;
    let max_theta = 150.0_f64;
    let mut lines = types::VectorOfVec4i::new();
    let red_color = Scalar::new(0.0, 0.0, 255.0, 1.0);
    let thickness = 5_i32;
    imgproc::hough_lines_p(
        &masked_edges,
        &mut lines,
        rho,
        theta,
        threshold,
        min_theta,
        max_theta,
    )
    .unwrap();
    for l in lines.iter() {
        imgproc::line(
            &mut img_result,
            core::Point::new(l[0], l[1]),
            core::Point::new(l[2], l[3]),
            red_color,
            thickness,
            imgproc::LINE_8,
            0,
        )
        .unwrap();
    }

    // Load an image
    highgui::imshow("original", &img_original).unwrap();
    highgui::imshow("gray", &img_gray).unwrap();
    highgui::imshow("gauss", &img_gauss).unwrap();
    highgui::imshow("canny", &img_canny).unwrap();
    highgui::imshow("masked edges", &masked_edges).unwrap();
    highgui::imshow("result", &img_result).unwrap();
    highgui::wait_key(0).unwrap();
}
