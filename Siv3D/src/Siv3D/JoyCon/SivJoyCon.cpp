//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2019 Ryo Suzuki
//	Copyright (c) 2016-2019 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------
// s3d::detail::DrawJoyCon* is originally created by azaika (OpenSiv3D Project)
//-----------------------------------------------

# include <Siv3D/JoyCon.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Circle.hpp>
# include <Siv3D/Rectangle.hpp>
# include <Siv3D/RoundRect.hpp>
# include <Siv3D/Transformer2D.hpp>
# include <Siv3D/Circular.hpp>
# include <Siv3D/Shape2D.hpp>

namespace s3d
{
	namespace detail
	{
		void DrawJoyConLShadowAt(const Vec2& center, const double _size, const double angle, const bool covered, const double offset, const double blurRadius, const double spread, const ColorF& color)
		{
			const Vec2 size = (covered ? Vec2(6.9, 15.33) : Vec2(5.27, 15.33));
			const RectF base(Arg::center = center, size);
			const RoundRect rrect = covered ? base.stretched(-0.4).rounded(3.0) : base.stretched(-0.3).rounded(2.7);

			Transformer2D t(Mat3x2::Scale(1.0 / 15.33 * _size, center).rotated(angle, center));
			rrect.drawShadow(Circular(offset, 180_deg - angle), blurRadius, spread, color);
		}

		void DrawJoyConLGuidedAt(const JoyCon& joycon, const JoyCon::ButtonGuide& guide, const Vec2& center, const double _size, const double angle, const bool covered, const ColorF& color, const ColorF& colorPushed, const ColorF& guideColor)
		{
			const Vec2 size = (covered ? Vec2(6.9, 15.33) : Vec2(5.27, 15.33));
			const RectF base(Arg::center = center, size);
			const ColorF defaultButtonColor(0.3);
			const auto buttonColor = [=](const Key & key, bool guide) {
				return (key.pressed() ? colorPushed : guide ? guideColor : defaultButtonColor);
			};

			Transformer2D t(Mat3x2::Scale(1.0 / 15.33 * _size, center).rotated(angle, center));

			// body
			{
				Circle(base.tl().movedBy(3.0, 3.6), 3.0).drawPie(0_deg, -90_deg, color);
				RectF(base.tl().movedBy(0.0, 3.6), Vec2(3.0, 8.63)).draw(color);
				Circle(base.bl().movedBy(3.0, -3.1), 3.0).drawPie(180_deg, 90_deg, color);
				RectF(base.tl().movedBy(3.00, 0.6), Vec2(1.82, 14.63)).draw(color);
			}

			// arrow
			{
				const double delta = 1.15;
				const Vec2 c = base.tl().movedBy(2.55, 8.33);

				Circle(c.movedBy(-delta, 0), 0.55).draw(buttonColor(joycon.button0, guide.button0)); // left
				Circle(c.movedBy(0, delta), 0.55).draw(buttonColor(joycon.button1, guide.button1));  // down
				Circle(c.movedBy(0, -delta), 0.55).draw(buttonColor(joycon.button2, guide.button2)); // up
				Circle(c.movedBy(delta, 0), 0.55).draw(buttonColor(joycon.button3, guide.button3));  // right
			}

			// stick
			{
				const Vec2 c = base.tl().movedBy(2.55, 4.28);
				Circle(c, 1.2).draw(buttonColor(joycon.buttonStick, guide.buttonStick));

				if (const auto dg = guide.povD8)
				{
					Circle(c + Circular(1.2, *dg * 45_deg - 90_deg), 0.6).draw(guideColor);
				}

				if (const auto d = joycon.povD8())
				{
					Circle(c + Circular(1.2, *d * 45_deg + 90_deg), 0.6).draw();
				}
			}

			// Screenshot
			RoundRect(Arg::center = base.tl().movedBy(3.4, 11.13), Vec2(1.0, 1.0), 0.1).draw(buttonColor(joycon.buttonScreenshot, guide.buttonScreenshot));

			// Minus
			RectF(Arg::center = base.tl().movedBy(4.03, 2.0), Vec2(0.8, 0.2)).draw(buttonColor(joycon.buttonMinus, guide.buttonMinus));

			// L, ZL
			RectF(base.tl().movedBy(3.0, 0.4), Vec2(0.9, 0.4)).draw(buttonColor(joycon.buttonLR, guide.buttonLR));
			Circle(base.tl().movedBy(3.0, 3.6), 3.0).drawArc(0_deg, -68_deg, 0.2, 0.2, buttonColor(joycon.buttonLR, guide.buttonLR));
			RectF(base.tl().movedBy(3.0, 0.0), Vec2(0.9, 0.4)).draw(buttonColor(joycon.buttonZLZR, guide.buttonZLZR));
			Circle(base.tl().movedBy(3.0, 3.6), 3.2).drawArc(0_deg, -30_deg, 0.0, 0.4, buttonColor(joycon.buttonZLZR, guide.buttonZLZR));

			if (covered)
			{
				// strap body
				Circle(base.tr().movedBy(-2.08, 2.33), 1.73).drawPie(0_deg, 90_deg, defaultButtonColor);
				RectF(base.tr().movedBy(-2.08, 2.33), Vec2(1.73, 11.0 + 0.17)).draw(defaultButtonColor);
				Circle(base.br().movedBy(-2.08, -2.0 + 0.17), 1.73).drawPie(90_deg, 90_deg, defaultButtonColor);

				// SL, SR
				RoundRect(base.tr().moveBy(-0.45, 3.1), Vec2(0.45, 2.0), 0.1).draw(buttonColor(joycon.buttonSL, guide.buttonSL));
				RoundRect(base.tr().moveBy(-0.45, 9.0), Vec2(0.45, 2.0), 0.1).draw(buttonColor(joycon.buttonSR, guide.buttonSR));
			}
			else
			{
				const auto buttonColor2 = [=](const Key & key, bool guide) {
					return (key.pressed() ? colorPushed : guide ? guideColor : color);
				};

				// SL, SR
				RectF(base.tr().movedBy(-0.45, 2.0), Vec2(0.3, 11.33)).draw(defaultButtonColor);
				RoundRect(base.tr().movedBy(-0.3, 3.5), Vec2(0.3, 1.2), 0.1).draw(buttonColor2(joycon.buttonSL, guide.buttonSL));
				RoundRect(base.tr().movedBy(-0.3, 9.4), Vec2(0.3, 1.2), 0.1).draw(buttonColor2(joycon.buttonSR, guide.buttonSR));
			}
		}


		void DrawJoyConRShadowAt(const Vec2& center, const double _size, const double angle, const bool covered, const double offset, const double blurRadius, const double spread, const ColorF& color)
		{
			const Vec2 size = (covered ? Vec2(6.9, 15.33) : Vec2(5.27, 15.33));
			const RectF base(Arg::center = center, size);
			const RoundRect rrect = covered ? base.stretched(-0.4).rounded(3.0) : base.stretched(-0.3).rounded(2.7);

			Transformer2D t(Mat3x2::Scale(1.0 / 15.33 * _size, center).rotated(angle, center));
			rrect.drawShadow(Circular(offset, 180_deg - angle), blurRadius, spread, color);
		}

		void DrawJoyConRGuidedAt(const JoyCon& joycon, const JoyCon::ButtonGuide& guide, const Vec2& center, const double _size, const double angle, const bool covered, const ColorF& color, const ColorF& colorPushed, const ColorF& guideColor)
		{
			const Vec2 size = (covered ? Vec2(6.9, 15.33) : Vec2(5.27, 15.33));
			const RectF base(Arg::center = center, size);
			const ColorF defaultButtonColor(0.3);
			const auto buttonColor = [=](const Key & key, bool guide) {
				return (key.pressed() ? colorPushed : guide ? guideColor : defaultButtonColor);
			};

			Transformer2D t(Mat3x2::Scale(1.0 / 15.33 * _size, center).rotated(angle, center));

			// body
			{
				Circle(base.tr().movedBy(-3.0, 3.6), 3.0).drawPie(0_deg, 90_deg, color);
				RectF(base.tr().movedBy(-3.0, 3.6), Vec2(3.0, 8.63)).draw(color);
				Circle(base.br().movedBy(-3.0, -3.1), 3.0).drawPie(180_deg, -90_deg, color);
				RectF(base.tr().movedBy(-4.82, 0.6), Vec2(1.82, 14.63)).draw(color);
			}

			// ABXY
			{
				const double delta = 1.15;
				const Vec2 c = base.tr().movedBy(-2.55, 4.28);

				Circle(c.movedBy(delta, 0), 0.55).draw(buttonColor(joycon.button0, guide.button0));  // A
				Circle(c.movedBy(0, -delta), 0.55).draw(buttonColor(joycon.button1, guide.button1)); // X
				Circle(c.movedBy(0, delta), 0.55).draw(buttonColor(joycon.button2, guide.button2));  // B
				Circle(c.movedBy(-delta, 0), 0.55).draw(buttonColor(joycon.button3, guide.button3)); // Y
			}

			// stick
			{
				const Vec2 c = base.tr().movedBy(-2.55, 8.33);
				Circle(c, 1.2).draw(buttonColor(joycon.buttonStick, guide.buttonStick));

				if (const auto dg = guide.povD8)
				{
					Circle(c + Circular(1.2, *dg * 45_deg - 90_deg), 0.6).draw(guideColor);
				}

				if (const auto d = joycon.povD8())
				{
					Circle(c + Circular(1.2, *d * 45_deg - 90_deg), 0.6).draw();
				}
			}

			// Home
			Circle(base.tr().movedBy(-3.4, 11.13), 0.55).draw(buttonColor(joycon.buttonHome, guide.buttonHome));

			// Plus
			Shape2D::Plus(0.4, 0.2, base.tr().movedBy(-4.03, 2.0)).draw(buttonColor(joycon.buttonPlus, guide.buttonPlus));

			// R, ZR
			RectF(base.tr().movedBy(-3.9, 0.4), Vec2(0.9, 0.4)).draw(buttonColor(joycon.buttonLR, guide.buttonLR));
			Circle(base.tr().movedBy(-3.0, 3.6), 3.0).drawArc(0_deg, 68_deg, 0.2, 0.2, buttonColor(joycon.buttonLR, guide.buttonLR));
			RectF(base.tr().movedBy(-3.9, 0.0), Vec2(0.9, 0.4)).draw(buttonColor(joycon.buttonZLZR, guide.buttonZLZR));
			Circle(base.tr().movedBy(-3.0, 3.6), 3.2).drawArc(0_deg, 30_deg, 0.0, 0.4, buttonColor(joycon.buttonZLZR, guide.buttonZLZR));

			if (covered)
			{
				// strap body
				Circle(base.tl().movedBy(2.08, 2.33), 1.73).drawPie(0_deg, -90_deg, defaultButtonColor);
				RectF(base.tl().movedBy(0.35, 2.33), Vec2(1.73, 11.0 + 0.17)).draw(defaultButtonColor);
				Circle(base.bl().movedBy(2.08, -2.0 + 0.17), 1.73).drawPie(180_deg, 90_deg, defaultButtonColor);

				// SL, SR
				RoundRect(base.tl().moveBy(0.0, 3.1), Vec2(0.45, 2.0), 0.1).draw(buttonColor(joycon.buttonSR, guide.buttonSR));
				RoundRect(base.tl().moveBy(0.0, 9.0), Vec2(0.45, 2.0), 0.1).draw(buttonColor(joycon.buttonSL, guide.buttonSL));
			}
			else
			{
				const auto buttonColor2 = [=](const Key & key, bool guide) {
					return (key.pressed() ? colorPushed : guide ? guideColor : color);
				};

				// SL, SR
				RectF(base.tl().movedBy(0.15, 2.0), Vec2(0.3, 11.33)).draw(defaultButtonColor);
				RoundRect(base.tl().movedBy(0.0, 3.5), Vec2(0.3, 1.2), 0.1).draw(buttonColor2(joycon.buttonSR, guide.buttonSR));
				RoundRect(base.tl().movedBy(0.0, 9.4), Vec2(0.3, 1.2), 0.1).draw(buttonColor2(joycon.buttonSL, guide.buttonSL));
			}
		}
	}

	bool JoyCon::IsJoyCon(const detail::Gamepad_impl& gamepad) noexcept
	{
		return (GetControllerType(gamepad.getInfo()) != JoyConType::Invalid);
	}

	bool JoyCon::IsJoyConL(const detail::Gamepad_impl& gamepad) noexcept
	{
		return (GetControllerType(gamepad.getInfo()) == JoyConType::JoyConL);
	}

	bool JoyCon::IsJoyConR(const detail::Gamepad_impl& gamepad) noexcept
	{
		return (GetControllerType(gamepad.getInfo()) == JoyConType::JoyConR);
	}

	JoyCon::JoyCon(const detail::Gamepad_impl& gamepad)
		: m_gamepadUserIndex(gamepad.userIndex)
		, m_joyconType(GetControllerType(gamepad.getInfo()))
	{
		if (m_joyconType != JoyConType::Invalid)
		{
			button0 = Key(InputDevice::Gamepad, 0, static_cast<uint8>(gamepad.userIndex));
			button1 = Key(InputDevice::Gamepad, 1, static_cast<uint8>(gamepad.userIndex));
			button2 = Key(InputDevice::Gamepad, 2, static_cast<uint8>(gamepad.userIndex));
			button3 = Key(InputDevice::Gamepad, 3, static_cast<uint8>(gamepad.userIndex));
			buttonSL = Key(InputDevice::Gamepad, 4, static_cast<uint8>(gamepad.userIndex));
			buttonSR = Key(InputDevice::Gamepad, 5, static_cast<uint8>(gamepad.userIndex));
			buttonMinus = Key(InputDevice::Gamepad, 8, static_cast<uint8>(gamepad.userIndex));
			buttonPlus = Key(InputDevice::Gamepad, 9, static_cast<uint8>(gamepad.userIndex));
			buttonHome = Key(InputDevice::Gamepad, 12, static_cast<uint8>(gamepad.userIndex));
			buttonScreenshot = Key(InputDevice::Gamepad, 13, static_cast<uint8>(gamepad.userIndex));
			buttonLR = Key(InputDevice::Gamepad, 14, static_cast<uint8>(gamepad.userIndex));
			buttonZLZR = Key(InputDevice::Gamepad, 15, static_cast<uint8>(gamepad.userIndex));
		}

		if (isJoyConL())
		{
			buttonStick = Key(InputDevice::Gamepad, 10, static_cast<uint8>(gamepad.userIndex));
		}
		else if (isJoyConR())
		{
			buttonStick = Key(InputDevice::Gamepad, 11, static_cast<uint8>(gamepad.userIndex));
		}
	}

	bool JoyCon::isConnected() const noexcept
	{
		return (m_joyconType != JoyConType::Invalid) && m_gamepadUserIndex
			&& Gamepad(m_gamepadUserIndex.value()).isConnected();
	}

	JoyCon::operator bool() const noexcept
	{
		return isConnected();
	}

	bool JoyCon::isJoyConL() const noexcept
	{
		return (m_joyconType == JoyConType::JoyConL);
	}

	bool JoyCon::isJoyConR() const noexcept
	{
		return (m_joyconType == JoyConType::JoyConR);
	}

	Optional<int32> JoyCon::povD8() const noexcept
	{
		if (!m_gamepadUserIndex)
		{
			return none;
		}

		return Gamepad(m_gamepadUserIndex.value()).povD8();
	}

	const JoyCon& JoyCon::drawShadowAt(const Vec2& center, const double _size, const double angle, const bool covered, const double offset, const double blurRadius, const double spread, const ColorF& color) const
	{
		switch (m_joyconType)
		{
		case JoyConType::JoyConL:
			detail::DrawJoyConLShadowAt(center, _size, angle, covered, offset, blurRadius, spread, color);
			break;
		case JoyConType::JoyConR:
			detail::DrawJoyConRShadowAt(center, _size, angle, covered, offset, blurRadius, spread, color);
			break;
		}

		return *this;
	}

	const JoyCon& JoyCon::drawAt(const Vec2& center, const double _size, const double angle, const bool covered, const Optional<ColorF>& color, const ColorF& colorPushed) const
	{
		return drawGuidedAt(ButtonGuide(), center, _size, angle, covered, color, colorPushed);
	}

	const JoyCon& JoyCon::drawGuidedAt(const ButtonGuide& guide, const Vec2& center, const double _size, const double angle, const bool covered, const Optional<ColorF>& color, const ColorF& colorPushed, const ColorF& guideColor) const
	{
		switch (m_joyconType)
		{
		case JoyConType::JoyConL:
			detail::DrawJoyConLGuidedAt(*this, guide, center, _size, angle, covered, color.value_or(ColorF(0.0, 0.75, 0.9)), colorPushed, guideColor);
			break;
		case JoyConType::JoyConR:
			detail::DrawJoyConRGuidedAt(*this, guide, center, _size, angle, covered, color.value_or(ColorF(1.0, 0.4, 0.3)), colorPushed, guideColor);
			break;
		}

		return *this;
	}

	JoyCon::JoyConType JoyCon::GetControllerType(const GamepadInfo& info) noexcept
	{
		if (info.vendorID == 0x57e)
		{
			if (info.productID == 0x2006)
			{
				return JoyConType::JoyConL;
			}
			else if (info.productID == 0x2007)
			{
				return JoyConType::JoyConR;
			}
		}

		return JoyConType::Invalid;
	}

	void JoyCon::DrawLShadowAt(const Vec2& center, const double size, const double angle, const bool covered, const double offset, const double blurRadius, const double spread, const ColorF& color)
	{
		detail::DrawJoyConLShadowAt(center, size, angle, covered, offset, blurRadius, spread, color);
	}

	void JoyCon::DrawLAt(const Vec2& center, const double size, const double angle, const bool covered, const ColorF& color, const ColorF& colorPushed)
	{
		DrawLGuidedAt(ButtonGuide(), center, size, angle, covered, color, colorPushed);
	}

	void JoyCon::DrawLGuidedAt(const JoyCon::ButtonGuide& guide, const Vec2& center, const double size, const double angle, const bool covered, const ColorF& color, const ColorF& colorPushed, const ColorF& guideColor)
	{
		detail::DrawJoyConLGuidedAt(JoyCon(), guide, center, size, angle, covered, color, colorPushed, guideColor);
	}

	void JoyCon::DrawRShadowAt(const Vec2& center, const double size, const double angle, const bool covered, const double offset, const double blurRadius, const double spread, const ColorF& color)
	{
		detail::DrawJoyConRShadowAt(center, size, angle, covered, offset, blurRadius, spread, color);
	}

	void JoyCon::DrawRAt(const Vec2& center, const double size, const double angle, const bool covered, const ColorF& color, const ColorF& colorPushed)
	{
		DrawRGuidedAt(ButtonGuide(), center, size, angle, covered, color, colorPushed);
	}

	void JoyCon::DrawRGuidedAt(const JoyCon::ButtonGuide& guide, const Vec2& center, const double size, const double angle, const bool covered, const ColorF& color, const ColorF& colorPushed, const ColorF& guideColor)
	{
		detail::DrawJoyConRGuidedAt(JoyCon(), guide, center, size, angle, covered, color, colorPushed, guideColor);
	}

	JoyCon JoyConL(const size_t index)
	{
		size_t count = 0;

		for (size_t i = 0; i < Gamepad.MaxUserCount; ++i)
		{
			const auto gamepad = Gamepad(i);

			if (JoyCon::IsJoyConL(gamepad))
			{
				if (count == index)
				{
					return JoyCon(gamepad);
				}

				++count;
			}
		}

		return JoyCon();
	}

	JoyCon JoyConR(const size_t index)
	{
		size_t count = 0;

		for (size_t i = 0; i < Gamepad.MaxUserCount; ++i)
		{
			const auto gamepad = Gamepad(i);

			if (JoyCon::IsJoyConR(gamepad))
			{
				if (count == index)
				{
					return JoyCon(gamepad);
				}

				++count;
			}
		}

		return JoyCon();
	}
}
