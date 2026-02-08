//
// Created by Doge on 08/02/2026.
//

#include "BSkia.h"

// Skia includes
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkPaint.h"
#include "include/core/SkStream.h"
#include "include/core/SkSurface.h"
#include "include/encode/SkPngEncoder.h"

#include <cstdio>

extern "C" {

bool BSkia_CreateTextPNG(const char* filename, int width, int height, const char* text) {
    if (!filename || !text || width <= 0 || height <= 0) {
        return false;
    }

    // Create an image surface
    sk_sp<SkSurface> surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(width, height));
    if (!surface) {
        fprintf(stderr, "Failed to create Skia surface\n");
        return false;
    }

    SkCanvas* canvas = surface->getCanvas();
    if (!canvas) {
        fprintf(stderr, "Failed to get canvas\n");
        return false;
    }

    // Clear background to white
    canvas->clear(SK_ColorWHITE);

    // Set up paint for text
    SkPaint paint;
    paint.setColor(SK_ColorBLACK);
    paint.setAntiAlias(true);

    // Set up font
    SkFont font;
    font.setSize(48.0f);

    // Draw text
    canvas->drawSimpleText(text, strlen(text), SkTextEncoding::kUTF8,
                          50, height / 2.0f, font, paint);

    // Draw a rectangle border
    SkPaint borderPaint;
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setColor(SK_ColorBLUE);
    borderPaint.setStrokeWidth(4.0f);
    canvas->drawRect(SkRect::MakeXYWH(10, 10, width - 20, height - 20), borderPaint);

    // Get the image
    sk_sp<SkImage> image = surface->makeImageSnapshot();
    if (!image) {
        fprintf(stderr, "Failed to create image snapshot\n");
        return false;
    }

    // Encode to PNG
    SkFILEWStream stream(filename);
    if (!stream.isValid()) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        return false;
    }

	SkPixmap pixmap;
	image->peekPixels(&pixmap);
	if (
		!SkPngEncoder::Encode(
			&stream,
			pixmap,
			{}
		)
	) {
		fprintf(stderr, "Failed to encode PNG\n");
		return false;
	}

    printf("Successfully created PNG: %s (%dx%d)\n", filename, width, height);
    return true;
}

const char* BSkia_GetVersion(void) {
    return "BSkia 1.0 with Skia (chrome/m122)";
}

} // extern "C"
