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
# include "NamedParameter.hpp"
# include "PredefinedNamedParameter.hpp"
# include "Unspecified.hpp"
# include "Grid.hpp"
# include "PointVector.hpp"
# include "Rectangle.hpp"
# include "ImageFormat.hpp"

namespace s3d
{
	enum class AdaptiveMethod
	{
		/// <summary>
		/// 
		/// </summary>
		Mean,

		/// <summary>
		/// 
		/// </summary>
		Gaussian,
	};

	enum class BorderType
	{
		Replicate,

		Reflect,

		Reflect_101,

		Default = Reflect_101,
	};

	/// <summary>
	/// 塗りつぶしの連結性
	/// </summary>
	enum class FloodFillConnectivity
	{
		/// <summary>
		/// 上下左右 4 ピクセル
		/// </summary>
		Value4 = 4,

		/// <summary>
		/// 周囲 8 ピクセル
		/// </summary>
		Value8 = 8,
	};

	/// <summary>
	/// 画像拡大縮小の手法
	/// </summary>
	enum class Interpolation
	{
		/// <summary>
		/// 最近傍補間
		/// </summary>
		Nearest,

		/// <summary>
		/// バイリニア補間
		/// </summary>
		Linear,

		/// <summary>
		/// バイキュービック補間
		/// </summary>
		Cubic,

		/// <summary>
		/// エリア（画像の大幅な縮小に適している）
		/// </summary>
		Area,

		/// <summary>
		/// Lanczos 法（拡大や、小幅な縮小に適している）
		/// </summary>
		Lanczos,

		/// <summary>
		/// 最適な手法を自動選択
		/// </summary>
		Unspecified,
	};

	enum class HaarCascade
	{
		Face,

		Eye,

		EyeWithEyeglasses,

		AnimeFace,

		FrontalCatFace,
	};

	/// <summary>
	/// 画像
	/// </summary>
	/// <remarks>
	/// メモリ上に確保される画像データです。
	/// ピクセル単位のアクセスや、変形、エフェクト処理を適用できます。
	/// イメージを描画する場合は Texture に変換する必要があります。
	/// </remarks>
	class Image
	{
	private:

		Array<Color> m_data;

		uint32 m_width = 0;

		uint32 m_height = 0;

		[[nodiscard]] static constexpr int32 Mod(int32 x, int32 y) noexcept
		{
			return (0 <= x) ? (x % y) : (y - ((-x - 1) % y) - 1);
		}

		[[nodiscard]] static constexpr int32 Mir(int32 x, int32 y) noexcept
		{
			const int32 t = Mod(x, y * 2);

			return (t < y) ? t : (y * 2 - 1) - t;
		}

		[[nodiscard]] static constexpr double Biliner(double c1, double c2, double c3, double c4, double px, double py)
		{
			return px * py * (c1 - c2 - c3 + c4) + px * (c2 - c1) + py * (c3 - c1) + c1;
		}

	public:

		/// <summary>
		/// 画像の最大サイズ
		/// </summary>
		static constexpr int32 MaxWidth = 8192;

		static constexpr int32 MaxHeight = 8192;

		using iterator					= Array<Color>::iterator;
		using const_iterator			= Array<Color>::const_iterator;
		using reverse_iterator			= Array<Color>::reverse_iterator;
		using const_reverse_iterator	= Array<Color>::const_reverse_iterator;

		template <class Fty, std::enable_if_t<std::is_invocable_r_v<Color, Fty>>* = nullptr>
		static Image Generate(const Size& size, Fty generator)
		{
			return Generate(size.x, size.y, generator);
		}

		template <class Fty, std::enable_if_t<std::is_invocable_r_v<Color, Fty>>* = nullptr>
		static Image Generate(const size_t width, const size_t height, Fty generator)
		{
			Image new_image(width, height);

			if (new_image.isEmpty())
			{
				return new_image;
			}

			Color* pDst = new_image[0];

			for (uint32 y = 0; y < height; ++y)
			{
				for (uint32 x = 0; x < width; ++x)
				{
					(*pDst++) = generator();
				}
			}

			return new_image;
		}

		template <class Fty, std::enable_if_t<std::is_invocable_r_v<Color, Fty, Point>>* = nullptr>
		static Image Generate(const Size& size, Fty generator)
		{
			return Generate(size.x, size.y, generator);
		}

		template <class Fty, std::enable_if_t<std::is_invocable_r_v<Color, Fty, Point>>* = nullptr>
		static Image Generate(const size_t width, const size_t height, Fty generator)
		{
			Image new_image(width, height);

			if (new_image.isEmpty())
			{
				return new_image;
			}

			Color* pDst = new_image[0];

			for (uint32 y = 0; y < height; ++y)
			{
				for (uint32 x = 0; x < width; ++x)
				{
					(*pDst++) = generator({ x, y });
				}
			}

			return new_image;
		}

		template <class Fty, std::enable_if_t<std::is_invocable_r_v<Color, Fty, Vec2>>* = nullptr>
		static Image Generate0_1(const Size& size, Fty generator)
		{
			return Generate0_1(size.x, size.y, generator);
		}

		template <class Fty, std::enable_if_t<std::is_invocable_r_v<Color, Fty, Vec2>>* = nullptr>
		static Image Generate0_1(const size_t width, const size_t height, Fty generator)
		{
			Image new_image(width, height);

			if (new_image.isEmpty())
			{
				return new_image;
			}

			const double sx = 1.0 / (width - 1);
			const double sy = 1.0 / (height - 1);
			Color* pDst = new_image[0];

			for (uint32 y = 0; y < height; ++y)
			{
				for (uint32 x = 0; x < width; ++x)
				{
					(*pDst++) = generator({ sx * x, sy * y });
				}
			}

			return new_image;
		}

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Image() = default;

		/// <summary>
		/// 画像を別の画像からコピーして作成します。
		/// </summary>
		/// <param name="image">
		/// コピーする画像
		/// </param>
		Image(const Image& image) = default;

		/// <summary>
		/// 画像を別の画像からムーブして作成します。
		/// </summary>
		/// <param name="image">
		/// ムーブする画像
		/// </param>
		Image(Image&& image) noexcept;

		/// <summary>
		/// 指定したサイズの画像を作成します。
		/// </summary>
		/// <param name="size">
		/// 画像のサイズ（ピクセル）
		/// </param>
		explicit Image(size_t size);

		/// <summary>
		/// 指定した色、サイズで塗りつぶされた画像を作成します。
		/// </summary>
		/// <param name="size">
		/// 画像のサイズ（ピクセル）
		/// </param>
		/// <param name="color">
		/// 塗りつぶしの色
		/// </param>
		explicit Image(size_t size, const Color& color);

		/// <summary>
		/// 指定したサイズの画像を作成します。
		/// </summary>
		/// <param name="size">
		/// 画像のサイズ（ピクセル）
		/// </param>
		explicit Image(const Size& size);

		/// <summary>
		/// 指定した色、サイズで塗りつぶされた画像を作成します。
		/// </summary>
		/// <param name="size">
		/// 画像のサイズ（ピクセル）
		/// </param>
		/// <param name="color">
		/// 塗りつぶしの色
		/// </param>
		Image(const Size& size, const Color& color);

		Image(const Size& size, Arg::generator_<std::function<Color(void)>> generator);

		Image(const Size& size, Arg::generator_<std::function<Color(Point)>> generator);

		Image(const Size& size, Arg::generator_<std::function<Color(Vec2)>> generator);

		Image(const Size& size, Arg::generator0_1_<std::function<Color(Vec2)>> generator);

		/// <summary>
		/// 指定したサイズの画像を作成します。
		/// </summary>
		/// <param name="width">
		/// 画像の幅（ピクセル）
		/// </param>
		/// <param name="height">
		/// 画像の高さ（ピクセル）
		/// </param>
		Image(size_t width, size_t height);

		Image(size_t width, size_t height, Arg::generator_<std::function<Color(void)>> generator);

		Image(size_t width, size_t height, Arg::generator_<std::function<Color(Point)>> generator);

		Image(size_t width, size_t height, Arg::generator_<std::function<Color(Vec2)>> generator);

		Image(size_t width, size_t height, Arg::generator0_1_<std::function<Color(Vec2)>> generator);

		/// <summary>
		/// 指定した色、サイズで塗りつぶされた画像を作成します。
		/// </summary>
		/// <param name="width">
		/// 画像の幅（ピクセル）
		/// </param>
		/// <param name="height">
		/// 画像の高さ（ピクセル）
		/// </param>
		/// <param name="color">
		/// 塗りつぶしの色
		/// </param>
		Image(size_t width, size_t height, const Color& color);

		/// <summary>
		/// 画像ファイルから画像を作成します。
		/// </summary>
		/// <param name="path">
		/// 画像ファイルのパス
		/// </param>
		explicit Image(const FilePath& path);

		/// <summary>
		/// Reader から画像を作成します。
		/// </summary>
		/// <param name="reader">
		/// Reader
		/// </param>
		/// <param name="format">
		/// 画像のフォーマット
		/// </param>
		explicit Image(IReader&& reader, ImageFormat format = ImageFormat::Unspecified);

		/// <summary>
		/// 2 つの画像ファイルから画像を作成します。
		/// </summary>
		/// <param name="rgb">
		/// RGB を読み込む画像ファイルのパス
		/// </param>
		/// <param name="alpha">
		/// アルファ値を読み込む画像ファイルのパス
		/// </param>
		Image(const FilePath& rgb, const FilePath& alpha);

		/// <summary>
		/// 画像ファイルからアルファ値を作成し、画像を作成します。
		/// </summary>
		/// <param name="rgb">
		/// RGB 成分の色
		/// </param>
		/// <param name="alpha">
		/// アルファ値を読み込む画像ファイルのパス
		/// </param>
		/// <remarks>
		/// alpha の画像の R 成分を、テクスチャのアルファ値に設定します。
		/// 画像ファイルの読み込みに失敗した場合、空のテクスチャを作成します。
		/// </remarks>
		Image(const Color& rgb, const FilePath& alpha);

		explicit Image(const Emoji& emoji);

		explicit Image(const Icon& icon);

		explicit Image(const Grid<Color>& grid);

		explicit Image(const Grid<ColorF>& grid);

		template <class Type, class Fty, std::enable_if_t<std::is_invocable_r_v<Color, Fty, Type>>* = nullptr>
		explicit Image(const Grid<Type>& grid, Fty converter)
			: Image(grid.width(), grid.height())
		{
			if (m_data.empty())
			{
				return;
			}

			const Type* pSrc = grid.data();
			const Type* const pSrcEnd = pSrc + grid.size_elements();
			Color* pDst = &m_data[0];

			while (pSrc != pSrcEnd)
			{
				*pDst++ = converter(*pSrc++);
			}
		}

		/// <summary>
		/// 新しい画像を代入します。
		/// </summary>
		/// <param name="str">
		/// 新しい画像
		/// </param>
		/// <returns>
		/// *this
		/// </returns>
		Image& operator =(const Image& image) = default;

		/// <summary>
		/// 新しい画像を代入します。
		/// </summary>
		/// <param name="str">
		/// 新しい画像
		/// </param>
		/// <returns>
		/// *this
		/// </returns>
		Image& operator =(Image&& image);

		Image& assign(const Image& image);

		Image& assign(Image&& image);

		/// <summary>
		/// 画像の幅（ピクセル）
		/// </summary>
		[[nodiscard]] int32 width() const noexcept
		{
			return m_width;
		}

		/// <summary>
		/// 画像の高さ（ピクセル）
		/// </summary>
		[[nodiscard]] int32 height() const noexcept
		{
			return m_height;
		}

		/// <summary>
		/// 画像の幅と高さ（ピクセル）
		/// </summary>
		[[nodiscard]] Size size() const noexcept
		{
			return{ m_width, m_height };
		}

		/// <summary>
		/// 画像の各行のデータサイズ
		/// </summary>
		[[nodiscard]] uint32 stride() const noexcept
		{
			return m_width * sizeof(Color);
		}

		/// <summary>
		/// 画像のピクセル数
		/// </summary>
		[[nodiscard]] uint32 num_pixels() const noexcept
		{
			return m_width * m_height;
		}

		/// <summary>
		/// 画像のデータサイズ
		/// </summary>
		[[nodiscard]] uint32 size_bytes() const
		{
			return stride() * m_height;
		}

		/// <summary>
		/// 画像が空かどうかを示します。
		/// </summary>
		[[nodiscard]] bool isEmpty() const
		{
			return m_data.empty();
		}

		/// <summary>
		/// 画像が空ではないかを返します。
		/// </summary>
		/// <returns>
		/// 画像が空ではない場合 true, それ以外の場合は false
		/// </returns>
		[[nodiscard]] explicit operator bool() const
		{
			return !isEmpty();
		}

		/// <summary>
		/// 画像の不要なメモリ消費を削除します。
		/// </summary>
		/// <returns>
		/// なし
		/// </returns>
		void shrink_to_fit()
		{
			m_data.shrink_to_fit();
		}

		/// <summary>
		/// 画像を消去し、空の画像にします。
		/// </summary>
		/// <remarks>
		/// メモリを解放したい場合は shrink_to_fit() を呼びます。
		/// </remarks>
		/// <returns>
		/// なし
		/// </returns>
		void clear()
		{
			m_data.clear();

			m_width = m_height = 0;
		}

		void release();

		/// <summary>
		/// 画像を別の画像と交換します。
		/// </summary>
		/// <param name="image">
		/// 交換する画像
		/// </param>
		/// <returns>
		/// なし
		/// </returns>
		void swap(Image& image) noexcept;

		/// <summary>
		/// 画像をコピーした新しい画像を返します。
		/// </summary>
		/// <returns>
		/// コピーした新しい画像
		/// </returns>
		[[nodiscard]] Image cloned() const;

		/// <summary>
		/// 指定した行の先頭ポインタを返します。
		/// </summary>
		/// <param name="y">
		/// 位置(行)
		/// </param>
		/// <returns>
		/// image[y][x] で指定したピクセルにアクセスします。
		/// </returns>
		/// <returns>
		/// 指定した行の先頭ポインタ
		/// </returns>
		[[nodiscard]] Color* operator[](size_t y)
		{
			return m_data.data() + (m_width * y);
		}

		[[nodiscard]] Color& operator[](const Point& pos)
		{
			return *(m_data.data() + (static_cast<size_t>(m_width) * pos.y + pos.x));
		}

		/// <summary>
		/// 指定した行の先頭ポインタを返します。
		/// </summary>
		/// <param name="y">
		/// 位置(行)
		/// </param>
		/// <returns>
		/// image[y][x] で指定したピクセルにアクセスします。
		/// </returns>
		/// <returns>
		/// 指定した行の先頭ポインタ
		/// </returns>
		[[nodiscard]] const Color* operator[](size_t y) const
		{
			return m_data.data() + (m_width * y);
		}

		[[nodiscard]] const Color& operator[](const Point& pos) const
		{
			return *(m_data.data() + (static_cast<size_t>(m_width) * pos.y + pos.x));
		}

		/// <summary>
		/// 画像データの先頭のポインタを返します。
		/// </summary>
		/// <returns>
		/// 画像データの先頭へのポインタ
		/// </returns>
		[[nodiscard]] Color* data()
		{
			return m_data.data();
		}

		/// <summary>
		/// 画像データの先頭のポインタを返します。
		/// </summary>
		/// <returns>
		/// 画像データの先頭へのポインタ
		/// </returns>
		[[nodiscard]] const Color* data() const
		{
			return m_data.data();
		}

		/// <summary>
		/// 画像データの先頭のポインタを返します。
		/// </summary>
		/// <returns>
		/// 画像データの先頭へのポインタ
		/// </returns>
		[[nodiscard]] uint8* dataAsUint8()
		{
			return static_cast<uint8*>(static_cast<void*>(m_data.data()));
		}

		/// <summary>
		/// 画像データの先頭のポインタを返します。
		/// </summary>
		/// <returns>
		/// 画像データの先頭へのポインタ
		/// </returns>
		[[nodiscard]] const uint8* dataAsUint8() const
		{
			return static_cast<const uint8*>(static_cast<const void*>(&m_data[0]));
		}

		/// <summary>
		/// 画像の先頭位置のイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の先頭位置のイテレータ
		/// </returns>
		[[nodiscard]] iterator begin() noexcept { return m_data.begin(); }

		/// <summary>
		/// 画像の先頭位置のイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の先頭位置のイテレータ
		/// </returns>
		[[nodiscard]] const_iterator begin() const noexcept { return m_data.begin(); }

		/// <summary>
		/// 画像の先頭位置のイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の先頭位置のイテレータ
		/// </returns>
		[[nodiscard]] const_iterator cbegin() const noexcept { return m_data.cbegin(); }

		/// <summary>
		/// 画像の終了位置のイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の終了位置のイテレータ
		/// </returns>
		[[nodiscard]] iterator end() noexcept { return m_data.end(); }

		/// <summary>
		/// 画像の終了位置のイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の終了位置のイテレータ
		/// </returns>
		[[nodiscard]] const_iterator end() const noexcept { return m_data.end(); }

		/// <summary>
		/// 画像の終了位置のイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の終了位置のイテレータ
		/// </returns>
		[[nodiscard]] const_iterator cend() const noexcept { return m_data.cend(); }

		/// <summary>
		/// 画像の末尾へのリバースイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の末尾へのリバースイテレータ
		/// </returns>
		[[nodiscard]] reverse_iterator rbegin() noexcept { return m_data.rbegin(); }

		/// <summary>
		/// 画像の末尾へのリバースイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の末尾へのリバースイテレータ
		/// </returns>
		[[nodiscard]] const_reverse_iterator rbegin() const noexcept { return m_data.rbegin(); }

		/// <summary>
		/// 画像の末尾へのリバースイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の末尾へのリバースイテレータ
		/// </returns>
		[[nodiscard]] const_reverse_iterator crbegin() const noexcept { return m_data.crbegin(); }

		/// <summary>
		/// 画像の先頭の前へのリバースイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の先頭の前へのリバースイテレータ
		/// </returns>
		[[nodiscard]] reverse_iterator rend() noexcept { return m_data.rend(); }

		/// <summary>
		/// 画像の先頭の前へのリバースイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の先頭の前へのリバースイテレータ
		/// </returns>
		[[nodiscard]] const_reverse_iterator rend() const noexcept { return m_data.rend(); }

		/// <summary>
		/// 画像の先頭の前へのリバースイテレータを取得します。
		/// </summary>
		/// <returns>
		/// 画像の先頭の前へのリバースイテレータ
		/// </returns>
		[[nodiscard]] const_reverse_iterator crend() const noexcept { return m_data.crend(); }


		/// <summary>
		/// 画像を指定した色で塗りつぶします。
		/// </summary>
		/// <param name="color">
		/// 塗りつぶしの色
		/// </param>
		/// <returns>
		/// なし
		/// </returns>
		void fill(const Color& color);

		/// <summary>
		/// 画像のサイズを変更します。
		/// </summary>
		/// <param name="width">
		/// 新しい幅(ピクセル)
		/// </param>
		/// <param name="height">
		/// 新しい高さ(ピクセル)
		/// </param>
		/// <remarks>
		/// サイズの変更が必要ないときは何もしません。
		/// サイズが変更された場合、すべての要素の値が不定になります。
		/// 画像を拡大縮小する場合は scale() を使ってください。
		/// </remarks>
		/// <returns>
		/// なし
		/// </returns>
		void resize(size_t width, size_t height);

		/// <summary>
		/// 画像のサイズを変更します。
		/// </summary>
		/// <param name="size">
		/// 新しいサイズ(ピクセル)
		/// </param>
		/// <remarks>
		/// サイズの変更が必要ないときは何もしません。
		/// サイズが変更された場合、すべての要素の値が不定になります。
		/// 画像を拡大縮小する場合は scale() を使ってください。
		/// </remarks>
		/// <returns>
		/// なし
		/// </returns>
		void resize(const Size& size)
		{
			resize(size.x, size.y);
		}

		/// <summary>
		/// 画像のサイズを変更します。
		/// </summary>
		/// <param name="width">
		/// 新しい幅(ピクセル)
		/// </param>
		/// <param name="height">
		/// 新しい高さ(ピクセル)
		/// </param>
		/// <param name="fillColor">
		/// 塗りつぶしの色
		/// </param>
		/// <remarks>
		/// サイズの変更が必要ないときは何もしません。
		/// サイズが変更された場合、すべての要素が fillColor で塗りつぶされます。
		/// 画像を拡大縮小する場合は scale() を使ってください。
		/// </remarks>
		/// <returns>
		/// なし
		/// </returns>
		void resize(size_t width, size_t height, const Color& fillColor);

		/// <summary>
		/// 画像のサイズを変更します。
		/// </summary>
		/// <param name="size">
		/// 新しいサイズ(ピクセル)
		/// </param>
		/// <param name="fillColor">
		/// 塗りつぶしの色
		/// </param>
		/// <remarks>
		/// サイズの変更が必要ないときは何もしません。
		/// サイズが変更された場合、すべての要素が fillColor で塗りつぶされます。
		/// 画像を拡大縮小する場合は scale() を使ってください。
		/// </remarks>
		/// <returns>
		/// なし
		/// </returns>
		void resize(const Size& size, const Color& fillColor)
		{
			resize(size.x, size.y, fillColor);
		}

		void resizeRows(size_t rows, const Color& fillColor);


		[[nodiscard]] const Color& getPixel_Repeat(int32 x, int32 y) const
		{
			return m_data[static_cast<size_t>(m_width) * Mod(y, m_height) + Mod(x, m_width)];
		}

		[[nodiscard]] const Color& getPixel_Repeat(const Point& pos) const
		{
			return getPixel_Repeat(pos.x, pos.y);
		}

		[[nodiscard]] const Color& getPixel_Clamp(int32 x, int32 y) const
		{
			x = Clamp(x, 0, static_cast<int32>(m_width) - 1);

			y = Clamp(y, 0, static_cast<int32>(m_height) - 1);

			return m_data[static_cast<size_t>(m_width) * y + x];
		}

		[[nodiscard]] const Color& getPixel_Clamp(const Point& pos) const
		{
			return getPixel_Clamp(pos.x, pos.y);
		}

		[[nodiscard]] const Color& getPixel_Mirror(int32 x, int32 y) const
		{
			x = Mir(x, m_width);

			y = Mir(y, m_height);

			return m_data[static_cast<size_t>(m_width) * y + x];
		}

		[[nodiscard]] const Color& getPixel_Mirror(const Point& pos) const
		{
			return getPixel_Mirror(pos.x, pos.y);
		}

		[[nodiscard]] ColorF sample_Repeat(double x, double y) const;

		[[nodiscard]] ColorF sample_Repeat(const Vec2& pos) const
		{
			return sample_Repeat(pos.x, pos.y);
		}

		[[nodiscard]] ColorF sample_Clamp(double x, double y) const;

		[[nodiscard]] ColorF sample_Clamp(const Vec2& pos) const
		{
			return sample_Clamp(pos.x, pos.y);
		}

		[[nodiscard]] ColorF sample_Mirror(double x, double y) const;

		[[nodiscard]] ColorF sample_Mirror(const Vec2& pos) const
		{
			return sample_Mirror(pos.x, pos.y);
		}

		/// <summary>
		/// 画像の一部分をコピーした新しい画像を返します。
		/// </summary>
		/// <param name="rect">
		/// 画像上の範囲
		/// </param>
		/// <returns>
		/// 一部分をコピーした新しい画像
		/// </returns>
		[[nodiscard]] Image clipped(const Rect& rect) const;

		/// <summary>
		/// 画像の一部分をコピーした新しい画像を返します。
		/// </summary>
		/// <param name="x">
		/// 画像上の範囲の左上 X 座標
		/// </param>
		/// <param name="y">
		/// 画像上の範囲の左上 Y 座標
		/// </param>
		/// <param name="w">
		/// 画像上の範囲の幅
		/// </param>
		/// <param name="h">
		/// 画像上の範囲の高さ
		/// </param>
		/// <returns>
		/// 一部分をコピーした新しい画像
		/// </returns>
		[[nodiscard]] Image clipped(int32 x, int32 y, int32 w, int32 h) const;

		[[nodiscard]] Image clipped(const Point& pos, int32 w, int32 h) const;

		[[nodiscard]] Image clipped(int32 x, int32 y, const Size& size) const;

		[[nodiscard]] Image clipped(const Point& pos, const Size& size) const;

		[[nodiscard]] Image squareClipped() const;

		/// <summary>
		/// すべてのピクセルに変換関数を適用します。
		/// </summary>
		/// <param name="function">
		/// 変換関数
		/// </param>
		/// <returns>
		/// *this
		/// </returns>
		Image& forEach(std::function<void(Color&)> function);

		Image& swapRB();

		bool applyAlphaFromRChannel(const FilePath& alpha);

		bool save(const FilePath& path, ImageFormat format = ImageFormat::Unspecified) const;

		bool saveWithDialog() const;

		bool savePNG(const FilePath& path, PNGFilter::Flag filterFlag = PNGFilter::Default) const;

		bool saveJPEG(const FilePath& path, int32 quality = 90) const;

		bool savePPM(const FilePath& path, PPMType format = PPMType::AsciiRGB) const;

		bool saveWebP(const FilePath& path, bool lossless = false, double quality = 90.0, WebPMethod method = WebPMethod::Default) const;

		[[nodiscard]] ByteArray encode(ImageFormat format = ImageFormat::PNG) const;

		[[nodiscard]] ByteArray encodePNG(PNGFilter::Flag filterFlag = PNGFilter::Default) const;

		[[nodiscard]] ByteArray encodeJPEG(int32 quality = 90) const;

		[[nodiscard]] ByteArray encodePPM(PPMType format = PPMType::AsciiRGB) const;

		[[nodiscard]] ByteArray encodeWebP(bool lossless = false, double quality = 90.0, WebPMethod method = WebPMethod::Default) const;

		/// <summary>
		/// ネガポジ反転処理を行います。
		/// </summary>
		/// <returns>
		/// *this
		/// </returns>
		Image& negate();

		/// <summary>
		/// ネガポジ反転処理した画像を返します。
		/// </summary>
		/// <returns>
		/// ネガポジ反転処理した画像
		/// </returns>
		[[nodiscard]] Image negated() const;

		/// <summary>
		/// グレースケール変換を行います。
		/// </summary>
		/// <returns>
		/// *this
		/// </returns>
		Image& grayscale();

		/// <summary>
		/// グレースケール変換した画像を返します。
		/// </summary>
		/// <returns>
		/// グレースケール変換した画像
		/// </returns>
		[[nodiscard]] Image grayscaled() const;

		/// <summary>
		/// セピア調変換を行います。
		/// </summary>
		/// <param name="level">
		/// セピアの強さ [0,255]
		/// </param>
		/// <returns>
		/// *this
		/// </returns>
		Image& sepia(int32 level = 25);

		/// <summary>
		/// セピア調変換した画像を返します。
		/// </summary>
		/// <param name="level">
		/// セピアの強さ [0,255]
		/// </param>
		/// <returns>
		/// セピア調変換した画像
		/// </returns>
		[[nodiscard]] Image sepiaed(int32 level = 25) const;

		/// <summary>
		/// ポスタライズ処理を行います。
		/// </summary>
		/// <param name="level">
		/// 各チャンネルの段階数
		/// </param>
		/// <returns>
		/// *this
		/// </returns>
		Image& postarize(int32 level);

		/// <summary>
		/// ポスタライズ処理を行った画像を返します。
		/// </summary>
		/// <param name="level">
		/// 各チャンネルの段階数
		/// </param>
		/// <returns>
		/// ポスタライズ処理を行った画像
		/// </returns>
		[[nodiscard]] Image postarized(int32 level) const;

		/// <summary>
		/// 明るさを変更します。
		/// </summary>
		/// <param name="level">
		/// 変更レベル
		/// </param>
		/// <returns>
		/// *this
		/// </returns>
		Image& brighten(int32 level);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="level">
		/// 
		/// </param>
		/// <returns>
		/// 
		/// </returns>
		[[nodiscard]] Image brightened(int32 level) const;

		Image& mirror();

		[[nodiscard]] Image mirrored() const;

		Image& flip();

		[[nodiscard]] Image flipped() const;

		Image& rotate90();

		[[nodiscard]] Image rotated90() const;

		Image& rotate180();

		[[nodiscard]] Image rotated180() const;

		Image& rotate270();

		[[nodiscard]] Image rotated270() const;

		Image& gammaCorrect(double gamma);

		[[nodiscard]] Image gammaCorrected(double gamma) const;

		Image& threshold(uint8 threshold, bool inverse = false);

		[[nodiscard]] Image thresholded(uint8 threshold, bool inverse = false) const;

		/// <summary>
		/// 適応的な閾値処理を行います。
		/// </summary>
		/// <param name="method">
		/// 適応的閾値アルゴリズム
		/// </param>
		/// <param name="blockSize">
		/// 閾値を求めるために利用される近傍領域のサイズ。奇数
		/// </param>
		/// <param name="c">
		/// 平均または加重平均から引かれる定数
		/// </param>
		/// <param name="inverse">
		/// 結果の白黒の反転
		/// </param>
		/// <remarks>
		/// https://docs.opencv.org/3.4.0/d7/d1b/group__imgproc__misc.html#ga72b913f352e4a1b1b397736707afcde3 参照
		/// </remarks>
		/// <returns>
		/// 
		/// </returns>
		Image& adaptiveThreshold(AdaptiveMethod method, int32 blockSize, double c, bool inverse = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="method">
		/// 適応的閾値アルゴリズム
		/// </param>
		/// <param name="blockSize">
		/// 閾値を求めるために利用される近傍領域のサイズ。奇数
		/// </param>
		/// <param name="c">
		/// 平均または加重平均から引かれる定数
		/// </param>
		/// <param name="inverse">
		/// 結果の白黒の反転
		/// </param>
		/// <remarks>
		/// https://docs.opencv.org/3.4.0/d7/d1b/group__imgproc__misc.html#ga72b913f352e4a1b1b397736707afcde3 参照
		/// </remarks>
		/// <returns>
		/// 
		/// </returns>
		[[nodiscard]] Image adaptiveThresholded(AdaptiveMethod method, int32 blockSize, double c, bool inverse = false) const;

		Image& mosaic(int32 size);

		Image& mosaic(int32 horizontal, int32 vertical);

		[[nodiscard]] Image mosaiced(int32 size) const;

		[[nodiscard]] Image mosaiced(int32 horizontal, int32 vertical) const;

		Image& spread(int32 size);

		Image& spread(int32 horizontal, int32 vertical);

		[[nodiscard]] Image spreaded(int32 size) const;

		[[nodiscard]] Image spreaded(int32 horizontal, int32 vertical) const;

		Image& blur(int32 size);

		Image& blur(int32 horizontal, int32 vertical);

		[[nodiscard]] Image blurred(int32 size) const;

		[[nodiscard]] Image blurred(int32 horizontal, int32 vertical) const;

		Image& medianBlur(int32 apertureSize);

		[[nodiscard]] Image medianBlurred(int32 apertureSize) const;

		Image& gaussianBlur(int32 size, BorderType borderType = BorderType::Default);

		Image& gaussianBlur(int32 horizontal, int32 vertical, BorderType borderType = BorderType::Default);

		[[nodiscard]] Image gaussianBlurred(int32 size, BorderType borderType = BorderType::Default) const;

		[[nodiscard]] Image gaussianBlurred(int32 horizontal, int32 vertical, BorderType borderType = BorderType::Default) const;

		Image& dilate(int32 iterations = 1);

		[[nodiscard]] Image dilated(int32 iterations = 1) const;

		Image& erode(int32 iterations = 1);

		[[nodiscard]] Image eroded(int32 iterations = 1) const;

		Image& floodFill(const Point& pos, const Color& color, FloodFillConnectivity connectivity = FloodFillConnectivity::Value4, int32 lowerDifference = 0, int32 upperDifference = 0);

		[[nodiscard]] Image floodFilled(const Point& pos, const Color& color, FloodFillConnectivity connectivity = FloodFillConnectivity::Value4, int32 lowerDifference = 0, int32 upperDifference = 0) const;

		Image& scale(int32 width, int32 height, Interpolation interpolation = Interpolation::Unspecified);

		[[nodiscard]] Image scaled(int32 width, int32 height, Interpolation interpolation = Interpolation::Unspecified) const;

		Image& scale(const Size& size, Interpolation interpolation = Interpolation::Unspecified);

		[[nodiscard]] Image scaled(const Size& size, Interpolation interpolation = Interpolation::Unspecified) const;

		Image& scale(double scaling, Interpolation interpolation = Interpolation::Unspecified);

		[[nodiscard]] Image scaled(double scaling, Interpolation interpolation = Interpolation::Unspecified) const;

		Image& fit(int32 width, int32 height, bool scaleUp = true, Interpolation interpolation = Interpolation::Unspecified);

		[[nodiscard]] Image fitted(int32 width, int32 height, bool scaleUp = true, Interpolation interpolation = Interpolation::Unspecified) const;

		Image& fit(const Size& size, bool scaleUp = true, Interpolation interpolation = Interpolation::Unspecified);

		[[nodiscard]] Image fitted(const Size& size, bool scaleUp = true, Interpolation interpolation = Interpolation::Unspecified) const;

		Image& border(int32 thickness, const Color& color = Palette::White);

		[[nodiscard]] Image bordered(int32 thickness, const Color& color = Palette::White) const;

		Image& border(int32 top, int32 right, int32 bottom, int32 left, const Color& color = Palette::White);

		[[nodiscard]] Image bordered(int32 top, int32 right, int32 bottom, int32 left, const Color& color = Palette::White) const;

		void paint(Image& dst, int32 x, int32 y, const Color& color = Palette::White) const;

		void paint(Image& dst, const Point& pos, const Color& color = Palette::White) const;

		void overwrite(Image& dst, int32 x, int32 y, const Color& color = Palette::White) const;

		void overwrite(Image& dst, const Point& pos, const Color& color = Palette::White) const;

		[[nodiscard]] ImageRegion operator ()(int32 x, int32 y, int32 w, int32 h) const;

		[[nodiscard]] ImageRegion operator ()(const Point& pos, int32 w, int32 h) const;

		[[nodiscard]] ImageRegion operator ()(int32 x, int32 y, const Size& size) const;

		[[nodiscard]] ImageRegion operator ()(const Point& pos, const Size& size) const;

		[[nodiscard]] ImageRegion operator ()(const Rect& rect) const;

		[[nodiscard]] Polygon alphaToPolygon(uint32 threshold = 160, bool allowHoles = true) const;

		[[nodiscard]] Polygon alphaToPolygonCentered(uint32 threshold = 160, bool allowHoles = true) const;

		[[nodiscard]] MultiPolygon alphaToPolygons(uint32 threshold = 160, bool allowHoles = true) const;

		[[nodiscard]] MultiPolygon alphaToPolygonsCentered(uint32 threshold = 160, bool allowHoles = true) const;

		[[nodiscard]] Polygon grayscaleToPolygon(uint32 threshold = 160, bool allowHoles = true) const;

		[[nodiscard]] Polygon grayscaleToPolygonCentered(uint32 threshold = 160, bool allowHoles = true) const;

		[[nodiscard]] MultiPolygon grayscaleToPolygons(uint32 threshold = 160, bool allowHoles = true) const;

		[[nodiscard]] MultiPolygon grayscaleToPolygonsCentered(uint32 threshold = 160, bool allowHoles = true) const;

		[[nodiscard]] Array<Rect> detectObjects(HaarCascade cascade, int32 minNeighbors = 3, const Size& minSize = Size(30, 30), const Optional<Size>& maxSize = unspecified) const;

		[[nodiscard]] Array<Rect> detectObjects(HaarCascade cascade, const Array<Rect>& regions, int32 minNeighbors = 3, const Size& minSize = Size(30, 30), const Optional<Size>& maxSize = unspecified) const;
	};
}
