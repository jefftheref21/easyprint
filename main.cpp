#include <iostream>
#include <opencv2/opencv.hpp>
#include <cairo.h>
#include <cairo-pdf.h>
#include <cmath>
#include <memory>

// Poppler
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>

static bool ends_with(const std::string& s, const std::string& suffix) {
    if (s.size() < suffix.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), s.rbegin(),
                      [](char a, char b) {
                          return std::tolower(a) == std::tolower(b);
                      });
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " input.(png|jpg|webp|pdf) [output.pdf] [num_pages] [images_per_page]\n";
        return 1;
    }

    std::string input_path = argv[1];
    std::string pdf_path   = (argc >= 3) ? argv[2] : "output.pdf";
    int pages             = (argc >= 4) ? std::atoi(argv[3]) : 1;
    int images_per_page    = (argc >= 5) ? std::atoi(argv[4]) : 1;

    if (pages < 1 || images_per_page < 1) {
        std::cerr << "Invalid pages or images_per_page\n";
        return 1;
    }

    cairo_surface_t* image_surface = nullptr;
    int img_width = 0;
    int img_height = 0;

    cv::Mat img_bgra;

    /* ---------- LOAD INPUT ---------- */

    if (ends_with(input_path, ".pdf")) {
        // -------- PDF INPUT --------
        std::unique_ptr<poppler::document> doc(
            poppler::document::load_from_file(input_path)
        );

        if (!doc) {
            std::cerr << "Failed to open PDF\n";
            return 1;
        }

        if (doc->pages() != 1) {
            std::cerr << "PDF must contain exactly one page\n";
            return 1;
        }

        std::unique_ptr<poppler::page> page(doc->create_page(0));
        if (!page) {
            std::cerr << "Failed to load PDF page\n";
            return 1;
        }

        poppler::page_renderer renderer;
        renderer.set_render_hint(poppler::page_renderer::antialiasing, true);
        renderer.set_render_hint(poppler::page_renderer::text_antialiasing, true);

        // Render at 300 DPI
        poppler::image pimg = renderer.render_page(page.get(), 300, 300);
        if (!pimg.is_valid()) {
            std::cerr << "Failed to render PDF\n";
            return 1;
        }

        img_width  = pimg.width();
        img_height = pimg.height();
        
        int stride = pimg.bytes_per_row();
        size_t size = stride * img_height;

        unsigned char* buffer = static_cast<unsigned char*>(malloc(size));
        if (!buffer) {
            std::cerr << "Out of memory\n";
            return 1;
        }

        std::memcpy(buffer, pimg.data(), size); 

        image_surface =
            cairo_image_surface_create_for_data(
                buffer,
                CAIRO_FORMAT_ARGB32,
                img_width,
                img_height,
                pimg.bytes_per_row()
            );

        static cairo_user_data_key_t buffer_key;

        cairo_surface_set_user_data(
            image_surface,
            &buffer_key,
            buffer,
            [](void* data) { free(data); }
        );
        
        std::cout << "Loaded PDF page\n";
    } else {
        // -------- IMAGE INPUT --------
        cv::Mat img = cv::imread(input_path, cv::IMREAD_COLOR);
        if (img.empty()) {
            std::cerr << "Failed to load image\n";
            return 1;
        }

        cv::cvtColor(img, img_bgra, cv::COLOR_BGR2BGRA);

        img_width  = img_bgra.cols;
        img_height = img_bgra.rows;

        image_surface =
            cairo_image_surface_create_for_data(
                img_bgra.data,
                CAIRO_FORMAT_ARGB32,
                img_width,
                img_height,
                img_bgra.step
            );
            
        std::cout << "Loaded image\n";
    }

    if (!image_surface ||
        cairo_surface_status(image_surface) != CAIRO_STATUS_SUCCESS) {
        std::cerr << "Failed to create Cairo image surface\n";
        return 1;
    }

    /* ---------- PAGE SETUP ---------- */

    const double page_width  = 612.0;  // 8.5 in
    const double page_height = 792.0;  // 11 in

    const double margin = 10.0;
    const double image_gap = 5.0;

    const double usable_width  = page_width  - 2 * margin;
    const double usable_height = page_height - 2 * margin;

    int cols = std::ceil(std::sqrt(images_per_page));
    int rows = std::ceil(static_cast<double>(images_per_page) / cols);

    double cell_width =
        (usable_width - (cols - 1) * image_gap) / cols;
    double cell_height =
        (usable_height - (rows - 1) * image_gap) / rows;
    
    cairo_surface_t* pdf_surface =
        cairo_pdf_surface_create(pdf_path.c_str(), page_width, page_height);
    if (cairo_surface_status(pdf_surface) != CAIRO_STATUS_SUCCESS) {
        std::cerr << "PDF surface error: "
                << cairo_status_to_string(cairo_surface_status(pdf_surface))
                << "\n";
        return 1;
    }
    cairo_t* cr = cairo_create(pdf_surface);
    if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        std::cerr << "Cairo context error: "
                << cairo_status_to_string(cairo_status(cr))
                << "\n";
        return 1;
    }

    /* ---------- DRAW ---------- */

    for (int p = 0; p < pages; ++p) {
        for (int i = 0; i < images_per_page; ++i) {
            int row = i / cols;
            int col = i % cols;

            double x = margin + col * (cell_width + image_gap);
            double y = margin + row * (cell_height + image_gap);

            double scale =
                std::min(cell_width / img_width,
                         cell_height / img_height);

            double draw_w = img_width  * scale;
            double draw_h = img_height * scale;

            double offset_x = (cell_width - draw_w) / 2.0;
            double offset_y = (cell_height - draw_h) / 2.0;

            cairo_save(cr);
            cairo_translate(cr, x + offset_x, y + offset_y);
            cairo_scale(cr, scale, scale);
            cairo_set_source_surface(cr, image_surface, 0, 0);
            cairo_paint(cr);
            cairo_restore(cr);
        }

        if (p < pages - 1) {
            cairo_show_page(cr);
        }
    }
    cairo_destroy(cr);
    cairo_surface_destroy(image_surface);
    cairo_surface_destroy(pdf_surface);

    std::cout << "Done\n";

    return 0;
}
