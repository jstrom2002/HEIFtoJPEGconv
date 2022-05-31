#pragma once
#include <libheif/heif.h>
#include <string>
#include <csetjmp>
#include <filesystem>
#include <stdio.h>

namespace HEIFtoJPEG
{
	class heif_converter
	{
	private:
		struct heif_context* ctx = nullptr;
		int option_aux = 0;
		int option_no_colons = 0;
		int quality_ = 90;

	public:
		void ConvertToJPEG(const char* input_filename);

		~heif_converter()
		{
			if(ctx != nullptr)
				heif_context_free(ctx);
		}

    private:

		void HEIF_CONVERTER_EXCEPTION(std::string str) {
			throw new std::exception(str.c_str());
		}

		heif_colorspace colorspace(bool has_alpha) const
		{
			return heif_colorspace_YCbCr;
		}

		heif_chroma chroma(bool has_alpha, int bit_depth) const
		{
			return heif_chroma_420;
		}

		void UpdateDecodingOptions(const struct heif_image_handle* handle,
			struct heif_decoding_options* options) const;

		bool Encode(const struct heif_image_handle* handle,
			const struct heif_image* image, const std::string& filename);

		static bool HasExifMetaData(const struct heif_image_handle* handle);
        static uint8_t* GetExifMetaData(const struct heif_image_handle* handle, size_t* size);
	};
}