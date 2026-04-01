#pragma once

struct IDWriteTextFormat;
struct ID2D1SolidColorBrush;

namespace gce
{

	class Font
	{
	public:
		Font() = default;
		Font(std::wstring_view name);
		Font(Font const& other);
		Font(Font&& other) = delete;

		Font& operator=(Font const&);
		Font& operator=(Font&&) = delete;

		~Font();

		void Load(std::wstring_view const& name);

	private:
		IDWriteTextFormat* m_format = nullptr;
		ID2D1SolidColorBrush* m_brush = nullptr;

		friend class Window;
	};
}
