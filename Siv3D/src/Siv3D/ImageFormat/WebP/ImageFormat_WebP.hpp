﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2019 Ryo Suzuki
//	Copyright (c) 2016-2019 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Image.hpp>
# include <Siv3D/ImageFormat.hpp>

namespace s3d
{
	class ImageFormat_WebP : public IImageFormat
	{
	public:

		ImageFormat format() const override;

		const Array<String>& possibleExtexsions() const override;

		bool isHeader(const uint8(&bytes)[16]) const override;

		Size getSize(const IReader& reader) const override;

		Image decode(IReader& reader) const override;

		bool encode(const Image& image, IWriter& writer) const override;

		bool encode(const Image& image, IWriter& writer, bool lossless, double quality, WebPMethod method) const;

		bool save(const Image& image, const FilePath& path) const override;
	};
}
