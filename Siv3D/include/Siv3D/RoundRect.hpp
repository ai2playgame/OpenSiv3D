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
# include "Fwd.hpp"
# include "PointVector.hpp"
# include "Rectangle.hpp"
# include "NamedParameter.hpp"

namespace s3d
{
	struct RoundRect
	{
		using position_type = RectF::position_type;

		using size_type = RectF::size_type;

		using value_type = position_type::value_type;

		SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			RectF rect;

			struct
			{
				value_type x, y, w, h;
			};
		};

		value_type r;

		SIV3D_DISABLE_MSVC_WARNINGS_POP()

		RoundRect() = default;

		constexpr RoundRect(value_type _x, value_type _y, value_type _w, value_type _h, value_type _r) noexcept
			: rect(_x, _y, _w, _h)
			, r(_r) {}

		constexpr RoundRect(const position_type& pos, value_type _w, value_type _h, value_type _r) noexcept
			: rect(pos, _w, _h)
			, r(_r) {}

		constexpr RoundRect(value_type _x, value_type _y, const size_type& size, value_type _r) noexcept
			: rect(_x, _y, size)
			, r(_r) {}

		constexpr RoundRect(const position_type& pos, const value_type& size, value_type _r) noexcept
			: rect(pos, size)
			, r(_r) {}

		constexpr RoundRect(const position_type& pos, const size_type& size, value_type _r) noexcept
			: rect(pos, size)
			, r(_r) {}

		constexpr RoundRect(Arg::center_<position_type> _center, value_type _w, value_type _h, value_type _r) noexcept
			: rect(_center, _w, _h)
			, r(_r) {}

		constexpr RoundRect(Arg::center_<position_type> _center, const size_type& size, value_type _r) noexcept
			: rect(_center, size)
			, r(_r) {}

		constexpr RoundRect(const RectF& _rect, value_type _r) noexcept
			: rect(_rect)
			, r(_r) {}

		constexpr RoundRect& set(value_type _x, value_type _y, value_type _w, value_type _h, value_type _r) noexcept
		{
			rect.set(_x, _y, _w, _h);
			r = _r;
			return *this;
		}

		constexpr RoundRect& set(const position_type& pos, value_type _w, value_type _h, value_type _r) noexcept
		{
			return set(pos.x, pos.y, _w, _h, _r);
		}

		constexpr RoundRect& set(value_type _x, value_type _y, const size_type& size, value_type _r) noexcept
		{
			return set(_x, _y, size.x, size.y, _r);
		}

		constexpr RoundRect& set(const position_type& pos, const size_type& size, value_type _r) noexcept
		{
			return set(pos.x, pos.y, size.x, size.y, _r);
		}

		constexpr RoundRect& set(const RectF& _rect, value_type _r) noexcept
		{
			return set(_rect.x, _rect.y, _rect.w, _rect.h, _r);
		}

		constexpr RoundRect& set(const RoundRect& roundRect) noexcept
		{
			return *this = roundRect;
		}

		constexpr RoundRect& setPos(value_type _x, value_type _y) noexcept
		{
			rect.setPos(_x, _y);
			return *this;
		}

		constexpr RoundRect& setPos(const position_type& _center) noexcept
		{
			return setPos(_center.x, _center.y);
		}

		constexpr RoundRect& setCenter(value_type _x, value_type _y) noexcept
		{
			rect.setCenter(_x, _y);
			return *this;
		}

		constexpr RoundRect& setCenter(const position_type& _center) noexcept
		{
			return setCenter(_center.x, _center.y);
		}

		constexpr RoundRect& setSize(value_type _w, value_type _h) noexcept
		{
			rect.setSize(_w, _h);
			return *this;
		}

		constexpr RoundRect& setSize(const size_type& _size) noexcept
		{
			return setSize(_size.x, _size.y);
		}

		[[nodiscard]] constexpr RoundRect movedBy(value_type _x, value_type _y) const noexcept
		{
			return{ rect.movedBy(_x, _y), r };
		}

		[[nodiscard]] constexpr RoundRect movedBy(const position_type& v) const noexcept
		{
			return movedBy(v.x, v.y);
		}

		constexpr RoundRect& moveBy(value_type _x, value_type _y) noexcept
		{
			rect.moveBy(_x, _y);
			return *this;
		}

		constexpr RoundRect& moveBy(const position_type& v) noexcept
		{
			return moveBy(v.x, v.y);
		}

		[[nodiscard]] constexpr RoundRect stretched(value_type size) const noexcept
		{
			return RoundRect(rect.stretched(size), r);
		}

		[[nodiscard]] constexpr RoundRect stretched(value_type _x, value_type _y) const noexcept
		{
			return RoundRect(rect.stretched(_x, _y), r);
		}

		[[nodiscard]] constexpr RoundRect stretched(const size_type& xy) const noexcept
		{
			return RoundRect(rect.stretched(xy.x, xy.y), r);
		}

		[[nodiscard]] constexpr RoundRect stretched(value_type top, value_type right, value_type bottom, value_type left) const noexcept
		{
			return RoundRect(rect.stretched(top, right, bottom, left), r);
		}

		[[nodiscard]] constexpr position_type center() const noexcept
		{
			return rect.center();
		}

		[[nodiscard]] constexpr double area() const noexcept
		{
			return rect.area() - (4 - Math::Pi) * r * r;
		}

		[[nodiscard]] constexpr double perimeter() const noexcept
		{
			return rect.perimeter() + r * (2 * Math::Pi - 8);
		}

		template <class Shape2DType>
		[[nodiscard]] bool intersects(const Shape2DType& shape) const
		{
			return Geometry2D::Intersect(*this, shape);
		}

		template <class Shape2DType>
		[[nodiscard]] bool contains(const Shape2DType& shape) const
		{
			return Geometry2D::Contains(*this, shape);
		}

		[[nodiscard]] bool leftClicked() const;

		[[nodiscard]] bool leftPressed() const;

		[[nodiscard]] bool leftReleased() const;

		[[nodiscard]] bool rightClicked() const;

		[[nodiscard]] bool rightPressed() const;

		[[nodiscard]] bool rightReleased() const;

		[[nodiscard]] bool mouseOver() const;

		const RoundRect& paint(Image& dst, const Color& color) const;

		const RoundRect& overwrite(Image& dst, const Color& color, bool antialiased = true) const;

		const RoundRect& paintFrame(Image& dst, int32 innerThickness, int32 outerThickness, const Color& color) const;

		const RoundRect& overwriteFrame(Image& dst, int32 innerThickness, int32 outerThickness, const Color& color, bool antialiased = true) const;

		const RoundRect& draw(const ColorF& color = Palette::White) const;

		const RoundRect& drawFrame(double thickness = 1.0, const ColorF& color = Palette::White) const
		{
			return drawFrame(thickness * 0.5, thickness * 0.5, color);
		}

		const RoundRect& drawFrame(double innerThickness, double outerThickness, const ColorF& color = Palette::White) const;

		/// <summary>
		/// 角丸長方形の影を描きます。
		/// </summary>
		/// <param name="offset">
		/// 影の移動量（ピクセル）
		/// </param>
		/// <param name="blurRadius">
		/// 影のぼかしの大きさ（ピクセル）
		/// </param>
		/// <param name="spread">
		/// 長方形の広がり（ピクセル）
		/// </param>
		/// <param name="color">
		/// 影の色
		/// </param>
		/// <returns>
		/// *this
		/// </returns>
		const RoundRect& drawShadow(const Vec2& offset, double blurRadius, double spread = 0.0, const ColorF& color = ColorF(0.0, 0.5)) const;

		[[nodiscard]] TexturedRoundRect operator ()(const Texture& texture) const;

		[[nodiscard]] TexturedRoundRect operator ()(const TextureRegion& textureRegion) const;

		[[nodiscard]] Polygon asPolygon() const;
	};
}

//////////////////////////////////////////////////
//
//	Format
//
//////////////////////////////////////////////////

namespace s3d
{
	void Formatter(FormatData& formatData, const RoundRect& value);

	template <class CharType>
	inline std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const RoundRect& value)
	{
		return	output << CharType('(')
			<< value.x << CharType(',') << CharType(' ')
			<< value.y << CharType(',') << CharType(' ')
			<< value.w << CharType(',') << CharType(' ')
			<< value.h << CharType(',') << CharType(' ')
			<< value.r << CharType(')');
	}

	template <class CharType>
	inline std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, RoundRect& value)
	{
		CharType unused;
		return	input >> unused
			>> value.x >> unused
			>> value.y >> unused
			>> value.w >> unused
			>> value.h >> unused
			>> value.r >> unused;
	}
}

//////////////////////////////////////////////////
//
//	Hash
//
//////////////////////////////////////////////////

namespace std
{
	template <>
	struct hash<s3d::RoundRect>
	{
		[[nodiscard]] size_t operator ()(const s3d::RoundRect& value) const noexcept
		{
			return s3d::Hash::FNV1a(value);
		}
	};
}

//////////////////////////////////////////////////
//
//	fmt
//
//////////////////////////////////////////////////

namespace fmt_s3d
{
	template <>
	struct formatter<s3d::RoundRect, s3d::char32>
	{
		s3d::String tag;

		template <class ParseContext>
		auto parse(ParseContext& ctx)
		{
			return s3d::detail::GetFmtTag(tag, ctx);
		}

		template <class Context>
		auto format(const s3d::RoundRect& value, Context& ctx)
		{
			const s3d::String fmt = s3d::detail::MakeFmtArg(
				U"({:", tag, U"}, {:", tag, U"}, {:", tag, U"}, {:", tag, U"}, {:", tag, U"})"
			);

			return format_to(ctx.begin(), wstring_view(fmt.data(), fmt.size()), value.x, value.y, value.w, value.h, value.r);
		}
	};
}
