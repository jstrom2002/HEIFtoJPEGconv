#include "heif_converter.h"
#include "Util.h"
#include <jpeglib.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <jpeglib.h>


namespace HEIFtoJPEG
{
    struct ErrorHandler
    {
        struct jpeg_error_mgr pub;  /* "public" fields */
        jmp_buf setjmp_buffer;  /* for return to caller */
    };

    void heif_converter::ConvertToJPEG(const char* input_filename)
    {
        std::string f_ext = getExtension(input_filename);
        if (f_ext != ".heic")
            return;

        std::string output_dir = getDirectory(input_filename);
        std::string output_filename = getFilename(input_filename);
        output_filename = output_filename.substr(0, output_filename.rfind(".")) + ".jpg";// remove extension, replace w/ jpeg.
        output_filename = output_dir + "\\" + output_filename;

        std::ifstream istr(input_filename, std::ios_base::binary);
        uint8_t magic[12];
        istr.read((char*)magic, 12);
        enum heif_filetype_result filetype_check = heif_check_filetype(magic, 12);
        if (filetype_check == heif_filetype_no) {
            fprintf(stderr, "Input file is not an HEIF/AVIF file\n");
            return;
        }

        if (filetype_check == heif_filetype_yes_unsupported) {
            fprintf(stderr, "Input file is an unsupported HEIF/AVIF file type\n");
            return;
        }



        // --- read the HEIF file

        ctx = heif_context_alloc();
        if (!ctx) {
            fprintf(stderr, "Could not create context object\n");
            return;
        }

        struct heif_error err;
        err = heif_context_read_from_file(ctx, input_filename, nullptr);
        if (err.code != 0) {
            HEIF_CONVERTER_EXCEPTION("Could not read HEIF/AVIF file: " + std::string(err.message) + "\n");
            return;
        }

        int num_images = heif_context_get_number_of_top_level_images(ctx);
        if (num_images == 0) {
            fprintf(stderr, "File doesn't contain any images\n");
            return;
        }

        std::vector<heif_item_id> image_IDs(num_images);
        num_images = heif_context_get_list_of_top_level_image_IDs(ctx, image_IDs.data(), num_images);


        std::string filename;
        size_t image_index = 1;  // Image filenames are "1" based.

        for (int idx = 0; idx < num_images; ++idx) {

            if (num_images > 1) {
                std::ostringstream s;
                s << output_filename.substr(0, output_filename.find_last_of('.'));
                s << "-" << image_index;
                s << output_filename.substr(output_filename.find_last_of('.'));
                filename.assign(s.str());
            }
            else {
                filename.assign(output_filename);
            }

            struct heif_image_handle* handle;
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
                //std::cerr << "Input image has undefined bit-depth\n";
                return;
            }

            struct heif_image* image;
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

            //// show decoding warnings
            //for (int i = 0;; i++) {
            //    int n = heif_image_get_decoding_warnings(image, i, &err, 1);
            //    if (n == 0) {
            //        break;
            //    }
            //    std::cerr << "Warning: " << err.message << "\n";
            //}

            if (image) {
                bool written = Encode(handle, image, filename);
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

                        std::ostringstream s;
                        s << output_filename.substr(0, output_filename.find('.'));
                        s << "-depth";
                        s << output_filename.substr(output_filename.find('.'));

                        written = Encode(depth_handle, depth_image, s.str());
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

                            std::ostringstream s;
                            s << output_filename.substr(0, output_filename.find('.'));
                            s << "-" + auxType;
                            s << output_filename.substr(output_filename.find('.'));

                            std::string auxFilename = s.str();

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
    }

    void heif_converter::UpdateDecodingOptions(const struct heif_image_handle* handle,
        struct heif_decoding_options* options) const
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
    bool heif_converter::HasExifMetaData(const struct heif_image_handle* handle)
    {

        heif_item_id metadata_id;
        int count = heif_image_handle_get_list_of_metadata_block_IDs(handle, kMetadataTypeExif,
            &metadata_id, 1);
        return count > 0;
    }

    // static
    uint8_t* heif_converter::GetExifMetaData(const struct heif_image_handle* handle, size_t* size)
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
}