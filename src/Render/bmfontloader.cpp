#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "bmfontloader.h"
#include "fileloader.h"
#include "debug.h"
#include "renderutil.h"
#include "strutil.h"

namespace
{
	SDL_Texture* texture_;
	SDL_Rect texture_rect_;
	bmfont::BMFontFile bmfile_;
	std::unordered_map<int, MappedTexture> mapped_textures_;
	bmfont::BMFontLine parseLine(const std::string& line)
	{
		std::stringstream ss{ line };
		std::string token;
		bmfont::BMFontLine bmfont_line;
		bool has_unclosed_quote{ false };
		std::string key;
		std::string value;
		while (ss >> token)
		{
			// Line type is always the first token
			if (bmfont_line.type.length() == 0)
			{
				bmfont_line.type = token;
				continue;
			}

			// Close any unclosed quote from previous token
			if (has_unclosed_quote)
			{
				bmfont_line.items[key] = strutil::strip(bmfont_line.items[key] + " " + token, "\"");
				has_unclosed_quote = (std::count(token.cbegin(), token.cend(), '\"') % 2 == 0); // check if there's still an unclosed quote
				continue;
			}

			unsigned int equals_pos = token.find_first_of('=');
			if (equals_pos == std::string::npos)
			{
				throw std::runtime_error("Error reading BMFont line [" + (line.length() > 0 ? line : "<empty>") + "]: expected equals sign in token [" + (token.length() > 0 ? token : "<empty>") + "]");
			}

			key = token.substr(0, equals_pos);
			value = token.substr(equals_pos + 1);

			// If token had an unclosed set of quotes ("), flag that the next token should be added to this
			has_unclosed_quote = (std::count(value.cbegin(), value.cend(), '\"') % 2 == 1);
			if (!has_unclosed_quote)
			{
				value = strutil::strip(value, "\"");
			}

			bmfont_line.items.emplace(key, value);
		}
		return bmfont_line;
	}
	std::vector<bmfont::BMFontLine> parseRawFile(std::string bmfont_path)
	{
		std::ifstream ifs{ bmfont_path, std::ios_base::in };
		if (!ifs.is_open())
		{
			throw std::runtime_error("Failed to load file [" + bmfont_path + "]: " + std::strerror(errno));
		}

		std::string line;
		std::vector<bmfont::BMFontLine> bmfont_lines;
		while (std::getline(ifs, line))
		{
			bmfont_lines.push_back(parseLine(line));
		}

		if (ifs.bad())
		{
			throw std::runtime_error("Opened but failed to read file [" + bmfont_path + "]: " + std::strerror(errno));
		}

		return bmfont_lines;
	}

	bmfont::BMFontFile parseLines(std::vector<bmfont::BMFontLine> lines)
	{
		bmfont::BMFontFile file;

		for (auto& line : lines)
		{
			if (line.type == "info")
			{
				file.info.face = line.items["face"];
				file.info.size = std::stoi(line.items["size"]);
				file.info.bold = std::stoi(line.items["bold"]) != 0;
				file.info.italic = std::stoi(line.items["italic"]) != 0;
				file.info.charset = line.items["charset"];
			}
			else if (line.type == "page")
			{
				file.page.id = std::stoi(line.items["id"]);
				file.page.filename = line.items["file"];
			}
			else if (line.type == "char")
			{
				bmfont::BMFontFile::CharLine char_line;
				char_line.id = std::stoi(line.items["id"]);
				char_line.x = std::stoi(line.items["x"]);
				char_line.y = std::stoi(line.items["y"]);
				char_line.w = std::stoi(line.items["width"]);
				char_line.h = std::stoi(line.items["height"]);
				char_line.page = std::stoi(line.items["page"]);
				char_line.xoffset = std::stoi(line.items["xoffset"]);
				char_line.yoffset = std::stoi(line.items["yoffset"]);
				char_line.xadvance = std::stoi(line.items["xadvance"]);
				char_line.chnl = std::stoi(line.items["chnl"]);
				file.chars[char_line.id] = char_line;
			}
		}
		return file;
	}

	//PageLine readPageLine(const std::string& line)
	//{
	//	try
	//	{
	//		PageLine page_line;
	//		std::string token;
	//		std::stringstream ss{ line };
	//		while (ss >> token)
	//		{
	//			if (token.substr(0, 2) == "id")
	//				page_line.id = std::stoi(token.substr(4, std::string::npos));
	//			if (token.substr(0, 4) == "file")
	//				page_line.filename = strutil::strip(token.substr(5, std::string::npos), "\"");
	//		}
	//		return page_line;
	//	}
	//	catch (std::exception e)
	//	{
	//		throw std::runtime_error("An error occurred while parsing the page line [" + line + "]: " + e.what());
	//	}
	//}
	//CharsLine readCharsLine(const std::string& line)
	//{
	//	try
	//	{
	//		CharsLine chars_line;
	//		std::string token;
	//		std::stringstream ss{ line };
	//	}
	//}
}

void bmfont::load(SDL_Renderer* sdl_renderer, const std::string& bmfont_path)
{
	debug::console({ "Loading mapped textures from BMFont file in ", bmfont_path });

	if (texture_)
	{
		SDL_DestroyTexture(texture_);
		texture_ = nullptr;
	}

	std::vector<BMFontLine> lines;
	try
	{
		lines = parseRawFile(bmfont_path);
	}
	catch (std::exception e)
	{
		program::exit(RetCode::runtime_error, { e.what() });
	}

	bmfile_ = parseLines(lines);

	texture_ = renderutil::loadTextureFromFile(sdl_renderer, "resources\\" + bmfile_.page.filename, &texture_rect_); // TODO: don't hardcode "resources\\", use same path as .fnt file

	for (auto& char_item : bmfile_.chars)
	{
		auto index = char_item.first;
		auto& item = char_item.second;
		auto texture = MappedTexture{ texture_, SDL_Rect{ item.x, item.y, item.w, item.h } };
		mapped_textures_[index] = texture;
	}

	debug::console({ "Loaded BMFont '", bmfont_path, "'" });
}

const std::unordered_map<int, MappedTexture>& bmfont::getMap()
{
	return mapped_textures_;
}

const bmfont::BMFontFile& bmfont::getDefinition()
{
	return bmfile_;
}