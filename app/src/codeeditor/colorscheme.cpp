#include "colorscheme.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#undef emit
#define RYML_SINGLE_HDR_DEFINE_NOW 
#include "../3rdparty/rapidyaml.hpp"
RESTORE_COMPILER_WARNINGS

ColorScheme ColorScheme::fromYaml(const QString& yaml)
{
	std::string stdyaml = yaml.toStdString();	
	ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(stdyaml)); // mutable (csubstr) overload

	const auto parseFormat = [&tree]<size_t N>(const char (&itemName) [N]) {
		Format f;
		const auto& item = tree[itemName];
		if (item.has_child("font"))
		{
			const c4::csubstr fontStyleText = item["font"].val();
			if (fontStyleText == "bold")
				f._weight = QFont::Bold;
			else if (fontStyleText == "light")
				f._weight = QFont::Light;
			else
				assert_unconditional_r("Unknown font weight value!");
		}

		const c4::csubstr colorText = item["color"].val();
		if (colorText[0] == '#')
		{
			const uint32_t color = std::stoul(std::string(colorText.data() + 1, colorText.size()), nullptr, 16);
			f._color = QColor::fromRgba(color | (0xFF << 24) /* alpha */);
		}
		else
			assert_unconditional_r("Unknown color format!");

		return f;
	};

	ColorScheme scheme;
	scheme._otherText = parseFormat("text");
	scheme._statementFormat = parseFormat("statements");
	scheme._numberFormat = parseFormat("numbers");
	scheme._preprocessorFormat = parseFormat("preprocessor");
	scheme._typesFormat = parseFormat("types");
	scheme._swizzleFormat = parseFormat("swizzle");
	scheme._commentFormat = parseFormat("comments");

	return scheme;
}
