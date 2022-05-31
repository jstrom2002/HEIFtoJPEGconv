#include "heif_converter.h"
#include "Util.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <libpng16/png.h>
#include <libpng16/pngconf.h>
#include <libpng16/pnglibconf.h>

namespace HEIFtoJPEG
{
    void heif_converter::Convert(const char* input_filename, int format_, float in_quality_, bool outputAux_)
    {
        // Early exit if not a HEIF file
        std::string f_ext = getExtension(input_filename);
        if (f_ext != ".heic")
            return;

        // Assign vars.
        filename = input_filename;
        quality_ = in_quality_;
        option_aux = outputAux_;
        output_dir = getDirectory(filename);
        switch (format_) {
        case 1:
            format = JPG;
            break;
        case 2:
            format = PNG;
            break;
        }


        // Validation using heif's built-in methods.
        std::ifstream istr(filename.c_str(), std::ios_base::in | std::ios_base::binary);
        uint8_t magic[12];
        istr.read((char*)magic, 12);
        enum heif_filetype_result filetype_check = heif_check_filetype(magic, 12);
        if (filetype_check == heif_filetype_no)
            HEIF_CONVERTER_EXCEPTION("Input file is not an HEIF/AVIF file\n");       

        if (filetype_check == heif_filetype_yes_unsupported) 
            HEIF_CONVERTER_EXCEPTION("Input file is an unsupported HEIF/AVIF file type\n");
        
        istr.close();



        // --- read the HEIF file
        ctx = heif_context_alloc();
        if (!ctx)
            HEIF_CONVERTER_EXCEPTION("Could not create context object\n");        

        err = heif_context_read_from_file(ctx, filename.c_str(), nullptr);
        if (err.code != 0)
            HEIF_CONVERTER_EXCEPTION("Could not read HEIF/AVIF file: " + std::string(err.message) + "\n");

        int num_images = heif_context_get_number_of_top_level_images(ctx);
        if (num_images == 0)
            HEIF_CONVERTER_EXCEPTION("File doesn't contain any images\n");        


        image_IDs.resize(num_images,0);
        num_images = heif_context_get_list_of_top_level_image_IDs(ctx, image_IDs.data(), num_images);

        for (int idx = 0; idx < num_images; ++idx) {
            generateOutputFilenames(idx);

            err = heif_context_get_image_handle(ctx, image_IDs[idx], &handle);
            if (err.code) {
                HEIF_CONVERTER_EXCEPTION("Could not read HEIF/AVIF image " + idx + std::string(": ") + err.message + "\n");
                return;
            }

            int has_alpha = heif_image_handle_has_alpha_channel(handle);
            struct heif_decoding_options* decode_options = heif_decoding_options_alloc();
            UpdateDecodingOptions(handle, decode_options);

            //decode_options->strict_decoding = strict_decoding;

            int bit_depth = heif_image_handle_get_luma_bits_per_pixel(handle);
            if (bit_depth < 0) {
                heif_decoding_options_free(decode_options);
                heif_image_handle_release(handle);
                HEIF_CONVERTER_EXCEPTION("Input image has undefined bit-depth\n");
            }

            err = heif_decode_image(handle,
                &image,
                colorspace(has_alpha),
                chroma(has_alpha, bit_depth),
                decode_options);
            heif_decoding_options_free(decode_options);
            if (err.code) {
                heif_image_handle_release(handle);
                HEIF_CONVERTER_EXCEPTION("Could not decode image: " + idx + std::string(": ") + err.message + "\n");
                return;
            }


            switch (format) {
            case JPG:
                ConvertToJPEG();
                break;
            case PNG:
                ConvertToPNG();
                break;
            }
        }
    }

    void heif_converter::ConvertToPNG()
    {
       png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
            nullptr, nullptr);
        if (!png_ptr) {
            HEIF_CONVERTER_EXCEPTION("libpng initialization failed (1)\n");
            return;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_write_struct(&png_ptr, nullptr);
            HEIF_CONVERTER_EXCEPTION("libpng initialization failed (2)\n");
            return;
        }

        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            HEIF_CONVERTER_EXCEPTION("Can't open " + std::string(filename.c_str()) + strerror(errno));
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return;
        }

        if (setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            fclose(fp);
            HEIF_CONVERTER_EXCEPTION( "Error while encoding image\n");
            return;
        }

        png_init_io(png_ptr, fp);

        bool withAlpha = (heif_image_get_chroma_format(image) == heif_chroma_interleaved_RGBA ||
            heif_image_get_chroma_format(image) == heif_chroma_interleaved_RRGGBBAA_BE);

        int width = heif_image_get_width(image, heif_channel_interleaved);
        int height = heif_image_get_height(image, heif_channel_interleaved);

        int bitDepth;
        int input_bpp = heif_image_get_bits_per_pixel_range(image, heif_channel_interleaved);
        if (input_bpp > 8) {
            bitDepth = 16;
        }
        else {
            bitDepth = 8;
        }

        const int colorType = withAlpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;

        png_set_IHDR(png_ptr, info_ptr, width, height, bitDepth, colorType,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        size_t profile_size = heif_image_handle_get_raw_color_profile_size(handle);
        if (profile_size > 0) {
            uint8_t* profile_data = static_cast<uint8_t*>(malloc(profile_size));
            heif_image_handle_get_raw_color_profile(handle, profile_data);
            char profile_name[] = "unknown";
            png_set_iCCP(png_ptr, info_ptr, profile_name, PNG_COMPRESSION_TYPE_BASE,
#if PNG_LIBPNG_VER < 10500
                (png_charp)profile_data,
#else
                (png_const_bytep)profile_data,
#endif
                (png_uint_32)profile_size);
            free(profile_data);
        }
        png_write_info(png_ptr, info_ptr);

        uint8_t** row_pointers = new uint8_t * [height];

        int stride_rgb;
        const uint8_t* row_rgb = heif_image_get_plane_readonly(image,
            heif_channel_interleaved, &stride_rgb);

        for (int y = 0; y < height; ++y) {
            row_pointers[y] = const_cast<uint8_t*>(&row_rgb[y * stride_rgb]);
        }

        if (bitDepth == 16) {
            // shift image data to full 16bit range

            int shift = 16 - input_bpp;
            if (shift > 0) {
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < stride_rgb; x += 2) {
                        uint8_t* p = (&row_pointers[y][x]);
                        int v = (p[0] << 8) | p[1];
                        v = (v << shift) | (v >> (16 - shift));
                        p[0] = (uint8_t)(v >> 8);
                        p[1] = (uint8_t)(v & 0xFF);
                    }
                }
            }
        }


        png_write_image(png_ptr, row_pointers);
        png_write_end(png_ptr, nullptr);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        delete[] row_pointers;
        fclose(fp);
    }

    void heif_converter::ConvertToJPEG()
    {
            if (image) {
                bool written = Encode(handle, image, output_filename);
                if (!written) {
                    HEIF_CONVERTER_EXCEPTION("could not write image\n");
                }
                heif_image_release(image);


                if (option_aux) {
                    int has_depth = heif_image_handle_has_depth_image(handle);
                    if (has_depth) {
                        heif_item_id depth_id;
                        int nDepthImages = heif_image_handle_get_list_of_depth_image_IDs(handle, &depth_id, 1);
                        assert(nDepthImages == 1);
                        (void)nDepthImages;

                        struct heif_image_handle* depth_handle;
                        err = heif_image_handle_get_depth_image_handle(handle, depth_id, &depth_handle);
                        if (err.code) {
                            heif_image_handle_release(handle);
                            HEIF_CONVERTER_EXCEPTION("Could not read depth channel");
                            return;
                        }

                        int depth_bit_depth = heif_image_handle_get_luma_bits_per_pixel(depth_handle);

                        struct heif_image* depth_image;
                        err = heif_decode_image(depth_handle,
                            &depth_image,
                            colorspace(false),
                            chroma(false, depth_bit_depth),
                            nullptr);
                        if (err.code) {
                            heif_image_handle_release(depth_handle);
                            heif_image_handle_release(handle);
                            HEIF_CONVERTER_EXCEPTION("Could not decode depth image: " + std::string(err.message));
                            return;
                        }

                        written = Encode(depth_handle, depth_image, output_filename);
                        if (!written) {
                            HEIF_CONVERTER_EXCEPTION("could not write depth image\n");
                        }

                        heif_image_release(depth_image);
                        heif_image_handle_release(depth_handle);
                    }
                }


                // --- aux images

                if (option_aux) {
                    int nAuxImages = heif_image_handle_get_number_of_auxiliary_images(handle, LIBHEIF_AUX_IMAGE_FILTER_OMIT_ALPHA | LIBHEIF_AUX_IMAGE_FILTER_OMIT_DEPTH);
                    if (nAuxImages > 0) {

                        std::vector<heif_item_id> auxIDs(nAuxImages);
                        heif_image_handle_get_list_of_auxiliary_image_IDs(handle,
                            LIBHEIF_AUX_IMAGE_FILTER_OMIT_ALPHA | LIBHEIF_AUX_IMAGE_FILTER_OMIT_DEPTH,
                            auxIDs.data(), nAuxImages);

                        for (heif_item_id auxId : auxIDs) {

                            struct heif_image_handle* aux_handle;
                            err = heif_image_handle_get_auxiliary_image_handle(handle, auxId, &aux_handle);
                            if (err.code) {
                                heif_image_handle_release(handle);
                                HEIF_CONVERTER_EXCEPTION("Could not read auxiliary image");
                                return;
                            }

                            int aux_bit_depth = heif_image_handle_get_luma_bits_per_pixel(aux_handle);

                            struct heif_image* aux_image;
                            err = heif_decode_image(aux_handle,
                                &aux_image,
                                colorspace(false),
                                chroma(false, aux_bit_depth),
                                nullptr);
                            if (err.code) {
                                heif_image_handle_release(aux_handle);
                                heif_image_handle_release(handle);
                                HEIF_CONVERTER_EXCEPTION("Could not decode auxiliary image: " + std::string(err.message));
                                return;
                            }

                            const char* auxTypeC = nullptr;
                            err = heif_image_handle_get_auxiliary_type(aux_handle, &auxTypeC);
                            if (err.code) {
                                heif_image_handle_release(aux_handle);
                                heif_image_handle_release(handle);
                                HEIF_CONVERTER_EXCEPTION("Could not get type of auxiliary image: " + std::string(err.message));
                                return;
                            }

                            std::string auxType = std::string(auxTypeC);

                            heif_image_handle_free_auxiliary_types(aux_handle, &auxTypeC);

                            std::string auxFilename = output_aux_filename;

                            if (option_no_colons) {
                                std::replace(auxFilename.begin(), auxFilename.end(), ':', '_');
                            }

                            written = Encode(aux_handle, aux_image, auxFilename);
                            if (!written) {
                                HEIF_CONVERTER_EXCEPTION("could not write auxiliary image\n");
                            }

                            heif_image_release(aux_image);
                            heif_image_handle_release(aux_handle);
                        }
                    }
                }

                heif_image_handle_release(handle);
            }
        image_index++;        
    }

    void UpdateDecodingOptions(const struct heif_image_handle* handle,  struct heif_decoding_options* options)
    {
        if (HasExifMetaData(handle)) {
            options->ignore_transformations = 1;
        }

        options->convert_hdr_to_8bit = 1;
    }

    // static
    static void OnJpegError(j_common_ptr cinfo)
    {
        ErrorHandler* handler = reinterpret_cast<ErrorHandler*>(cinfo->err);
        longjmp(handler->setjmp_buffer, 1);
    }


#define MAX_BYTES_IN_MARKER  65533      /* maximum data len of a JPEG marker */
#define ICC_MARKER  (JPEG_APP0 + 2)     /* JPEG marker code for ICC */
#define ICC_OVERHEAD_LEN  14            /* size of non-profile data in APP2 */
#define MAX_DATA_BYTES_IN_MARKER (MAX_BYTES_IN_MARKER - ICC_OVERHEAD_LEN)

    bool heif_converter::Encode(const struct heif_image_handle* handle,
        const struct heif_image* image, const std::string& filename)
    {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            fprintf(stderr, "Can't open %s: %s\n", filename.c_str(), strerror(errno));
            return false;
        }

        struct jpeg_compress_struct cinfo;
        struct ErrorHandler jerr;
        cinfo.err = jpeg_std_error(reinterpret_cast<struct jpeg_error_mgr*>(&jerr));
        jerr.pub.error_exit = &OnJpegError;
        if (setjmp(jerr.setjmp_buffer)) {
            cinfo.err->output_message(reinterpret_cast<j_common_ptr>(&cinfo));
            jpeg_destroy_compress(&cinfo);
            fclose(fp);
            return false;
        }

        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, fp);

        cinfo.image_width = heif_image_get_width(image, heif_channel_Y);
        cinfo.image_height = heif_image_get_height(image, heif_channel_Y);
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_YCbCr;
        jpeg_set_defaults(&cinfo);
        static const boolean kForceBaseline = TRUE;
        jpeg_set_quality(&cinfo, quality_, kForceBaseline);
        static const boolean kWriteAllTables = TRUE;
        jpeg_start_compress(&cinfo, kWriteAllTables);

        size_t exifsize = 0;
        uint8_t* exifdata = GetExifMetaData(handle, &exifsize);
        if (exifdata) {
            if (exifsize > 4) {
                static const uint8_t kExifMarker = JPEG_APP0 + 1;

                uint32_t skip = (exifdata[0] << 24) | (exifdata[1] << 16) | (exifdata[2] << 8) | exifdata[3];
                if (skip >= 6) {
                    skip = 4 + skip - 6;
                }
                else {
                    skip = 4;
                }

                uint8_t* ptr = exifdata + skip;
                size_t size = exifsize - skip;

                while (size > MAX_BYTES_IN_MARKER) {
                    jpeg_write_marker(&cinfo, kExifMarker, ptr,
                        static_cast<unsigned int>(MAX_BYTES_IN_MARKER));

                    ptr += MAX_BYTES_IN_MARKER;
                    size -= MAX_BYTES_IN_MARKER;
                }

                jpeg_write_marker(&cinfo, kExifMarker, ptr,
                    static_cast<unsigned int>(size));
            }

            free(exifdata);
        }

        size_t profile_size = heif_image_handle_get_raw_color_profile_size(handle);
        if (profile_size > 0) {
            uint8_t* profile_data = static_cast<uint8_t*>(malloc(profile_size));
            heif_image_handle_get_raw_color_profile(handle, profile_data);
            jpeg_write_icc_profile(&cinfo, profile_data, (unsigned int)profile_size);
            free(profile_data);
        }


        if (heif_image_get_bits_per_pixel(image, heif_channel_Y) != 8) {
            fprintf(stderr, "JPEG writer cannot handle image with >8 bpp.\n");
            return false;
        }


        int stride_y;
        const uint8_t* row_y = heif_image_get_plane_readonly(image, heif_channel_Y,
            &stride_y);
        int stride_u;
        const uint8_t* row_u = heif_image_get_plane_readonly(image, heif_channel_Cb,
            &stride_u);
        int stride_v;
        const uint8_t* row_v = heif_image_get_plane_readonly(image, heif_channel_Cr,
            &stride_v);

        JSAMPARRAY buffer = cinfo.mem->alloc_sarray(
            reinterpret_cast<j_common_ptr>(&cinfo), JPOOL_IMAGE,
            cinfo.image_width * cinfo.input_components, 1);
        JSAMPROW row[1] = { buffer[0] };

        while (cinfo.next_scanline < cinfo.image_height) {
            size_t offset_y = cinfo.next_scanline * stride_y;
            const uint8_t* start_y = &row_y[offset_y];
            size_t offset_u = (cinfo.next_scanline / 2) * stride_u;
            const uint8_t* start_u = &row_u[offset_u];
            size_t offset_v = (cinfo.next_scanline / 2) * stride_v;
            const uint8_t* start_v = &row_v[offset_v];

            JOCTET* bufp = buffer[0];
            for (JDIMENSION x = 0; x < cinfo.image_width; ++x) {
                *bufp++ = start_y[x];
                *bufp++ = start_u[x / 2];
                *bufp++ = start_v[x / 2];
            }
            jpeg_write_scanlines(&cinfo, row, 1);
        }
        jpeg_finish_compress(&cinfo);
        fclose(fp);
        jpeg_destroy_compress(&cinfo);
        return true;
    }

    static const char kMetadataTypeExif[] = "Exif";

    // static
    bool HasExifMetaData(const struct heif_image_handle* handle)
    {

        heif_item_id metadata_id;
        int count = heif_image_handle_get_list_of_metadata_block_IDs(handle, kMetadataTypeExif,
            &metadata_id, 1);
        return count > 0;
    }

    // static
    uint8_t* GetExifMetaData(const struct heif_image_handle* handle, size_t* size)
    {
        heif_item_id metadata_id;
        int count = heif_image_handle_get_list_of_metadata_block_IDs(handle, kMetadataTypeExif,
            &metadata_id, 1);

        for (int i = 0; i < count; i++) {
            size_t datasize = heif_image_handle_get_metadata_size(handle, metadata_id);
            uint8_t* data = static_cast<uint8_t*>(malloc(datasize));
            if (!data) {
                continue;
            }

            heif_error error = heif_image_handle_get_metadata(handle, metadata_id, data);
            if (error.code != heif_error_Ok) {
                free(data);
                continue;
            }

            *size = datasize;
            return data;
        }

        return nullptr;
    }

    void heif_converter::HEIF_CONVERTER_EXCEPTION(std::string str) {
        throw new std::exception(str.c_str());
    }

    void heif_converter::generateOutputFilenames(int i)
    {
        output_filename = getFilename(filename);
        output_filename = output_filename.substr(0, output_filename.rfind("."));
        output_aux_filename = output_filename;
        output_filename += "-" + std::to_string(i);
        output_aux_filename += "-aux" + std::to_string(i);
        appendFileExt(output_filename);
        appendFileExt(output_aux_filename);
        output_filename = output_dir + "\\" + output_filename;
        output_aux_filename = output_dir + "\\" + output_aux_filename;
    }

    void heif_converter::appendFileExt(std::string& str)
    {
        switch (format) {
        case JPG:
            str += ".jpg";
            break;
        case PNG:
            str += ".png";
            break;
        }
    }

    heif_colorspace heif_converter::colorspace(bool has_alpha) const
    {
        switch (format) {
        case JPG:
            return heif_colorspace_YCbCr;
        case PNG:
            return heif_colorspace_RGB;
        }

        return heif_colorspace_undefined;
    }

    heif_chroma heif_converter::chroma(bool has_alpha, int bit_depth) const
    {
        switch (format) {
        case JPG:
            return heif_chroma_420;
        case PNG:
            if (bit_depth == 8) {
                if (has_alpha)
                    return heif_chroma_interleaved_RGBA;
                else
                    return heif_chroma_interleaved_RGB;
            }
            else {
                if (has_alpha)
                    return heif_chroma_interleaved_RRGGBBAA_BE;
                else
                    return heif_chroma_interleaved_RRGGBB_BE;
            }
        }

        return heif_chroma_undefined;
    }
}