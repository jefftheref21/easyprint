#include <iostream>
#include <opencv2/opencv.hpp>
#include <cairo.h>
#include <cairo-pdf.h>
#include <cmath>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " image.png [output.pdf] [copies] [images_per_page]\n";
        return 1;
    }

    std::string image_path = argv[1];
    std::string pdf_path   = (argc >= 3) ? argv[2] : "output.pdf";
    int copies             = (argc >= 4) ? std::atoi(argv[3]) : 1;
    int images_per_page    = (argc >= 5) ? std::atoi(argv[4]) : 1;

    if (copies < 1 || images_per_page < 1) {
        std::cerr << "Invalid copies or images_per_page\n";
        return 1;
    }

    // Load image
    cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Failed to load image\n";
        return 1;
    }

    // Convert to BGRA for Cairo
    cv::Mat img_bgra;
    cv::cvtColor(img, img_bgra, cv::COLOR_BGR2BGRA);

    cairo_surface_t* image_surface =
        cairo_image_surface_create_for_data(
            img_bgra.data,
            CAIRO_FORMAT_ARGB32,
            img_bgra.cols,
            img_bgra.rows,
            img_bgra.step
        );

    if (cairo_surface_status(image_surface) != CAIRO_STATUS_SUCCESS) {
        std::cerr << "Failed to create Cairo image surface\n";
        return 1;
    }

    /* ---------- Page setup ---------- */

    const double page_width  = 612.0;  // US Letter: 8.5 in × 72
    const double page_height = 792.0;  // 11 in × 72

    const double image_gap = 5.0;


    const double margin_top    = 10.0;
    const double margin_left   = 10.0;
    const double margin_right  = 10.0;
    const double margin_bottom = 10.0;

    const double usable_width  =
        page_width - margin_left - margin_right;
    const double usable_height =
        page_height - margin_top - margin_bottom;

    // Grid layout
    int cols = std::ceil(std::sqrt(images_per_page));
    int rows = std::ceil(static_cast<double>(images_per_page) / cols);

    double cell_width =
    (usable_width - (cols - 1) * image_gap) / cols;

double cell_height =
    (usable_height - (rows - 1) * image_gap) / rows;


    cairo_surface_t* pdf_surface =
        cairo_pdf_surface_create(pdf_path.c_str(), page_width, page_height);
    cairo_t* cr = cairo_create(pdf_surface);

    for (int c = 0; c < copies; ++c) {
        for (int i = 0; i < images_per_page; ++i) {
            int row = i / cols;
            int col = i % cols;

            double x = margin_left + col * (cell_width + image_gap);
            double y = margin_top  + row * (cell_height + image_gap);


            double scale_x = cell_width  / img_bgra.cols;
            double scale_y = cell_height / img_bgra.rows;
            double scale = std::min(scale_x, scale_y);

            double draw_width  = img_bgra.cols * scale;
            double draw_height = img_bgra.rows * scale;

            double offset_x = (cell_width  - draw_width)  / 2.0;
            double offset_y = (cell_height - draw_height) / 2.0;

            cairo_save(cr);
            cairo_translate(cr, x + offset_x, y + offset_y);
            cairo_scale(cr, scale, scale);
            cairo_set_source_surface(cr, image_surface, 0, 0);
            cairo_paint(cr);
            cairo_restore(cr);
        }

        if (c < copies - 1) {
            cairo_show_page(cr); // new page
        }
    }

    // Cleanup (finalizes PDF)
    cairo_destroy(cr);
    cairo_surface_destroy(image_surface);
    cairo_surface_destroy(pdf_surface);

    return 0;
}
