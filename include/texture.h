#pragma once
#include "stdafx.h"
#include <string>
#include <opencv2/opencv.hpp>

namespace HEIFtoJPEG
{
	struct Texture 
	{
		GLuint id = 0;
		std::string filepath = "";

		Texture(cv::Mat& img, std::string filename)
		{
			this->filepath = filename;
			if (img.channels() != 4)
				cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
			int h = img.rows;
			int w = img.cols;
			unsigned int sz = img.total() * img.elemSize();
			uint8_t* data = new uint8_t[sz];
			memcpy(data, img.data, sz);
			img.deallocate();
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			if (this->id)
				glDeleteTextures(1, &this->id);
			glGenTextures(1, &this->id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void clear()
		{
			if (id)
				glDeleteTextures(1, &id);
			filepath = "";
		}
	};
}