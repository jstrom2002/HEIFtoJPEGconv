#pragma once
#include <libheif/heif.h>
#include <string>
#include <csetjmp>
#include <filesystem>
#include <stdio.h>
#include <jpeglib.h>

namespace HEIFtoJPEG
{
	struct ErrorHandler
	{
		struct jpeg_error_mgr pub;  /* "public" fields */
		jmp_buf setjmp_buffer;  /* for return to caller */
	};

	static bool HasExifMetaData(const struct heif_image_handle* handle);
	static uint8_t* GetExifMetaData(const struct heif_image_handle* handle, size_t* size);
	static void UpdateDecodingOptions(const struct heif_image_handle* handle, struct heif_decoding_options* options);

	enum FORMAT { UNKNOWN = 0u, JPG = 1u, PNG = 2u };

	class heif_converter
	{
	private:
		struct heif_context* ctx = nullptr;
		bool option_aux = 0;
		int option_no_colons = 0;
		int quality_ = 90;
		FORMAT format = UNKNOWN;
		std::string output_dir = "";
		std::string output_filename = "";
		std::string output_aux_filename = "";
		std::string filename = "";
		std::vector<heif_item_id> image_IDs;
		size_t image_index = 1;  // Image filenames are "1" based.
		struct heif_error err;
		struct heif_image_handle* handle = nullptr;
		struct heif_image* image = nullptr;

	public:
		void Convert(const char* filename, int format, float quality_ = 90, bool outputAux_ = false);

		~heif_converter()
		{
			if(ctx != nullptr)
				heif_context_free(ctx);
		}

    private:

		void ConvertToJPEG();
		void ConvertToPNG();
		void generateOutputFilenames(int i);
		void HEIF_CONVERTER_EXCEPTION(std::string str);
		void appendFileExt(std::string& str);
		heif_colorspace colorspace(bool has_alpha) const;
		heif_chroma chroma(bool has_alpha, int bit_depth) const;
		bool Encode(const struct heif_image_handle* handle, const struct heif_image* image, const std::string& filename);
	};
}