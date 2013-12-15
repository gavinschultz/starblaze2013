#pragma once
#include <SDL.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "mappedtexture.h"

namespace bmfont
{
	class BMFontLine
	{
	public:
		std::string type;
		std::unordered_map<std::string, std::string> items;
	};
	class BMFontFile
	{
	public:
		class InfoLine
		{
		public:
			std::string face;
			unsigned int size;
			bool bold;
			bool italic;
			std::string charset;
			//TODO: other properties
		};
		class PageLine
		{
		public:
			unsigned int id;
			std::string filename;
		};
		class CharLine
		{
		public:
			unsigned int id;
			unsigned int x;
			unsigned int y;
			unsigned int w;
			unsigned int h;
			int xoffset;
			int yoffset;
			int xadvance;
			unsigned int page;
			unsigned int chnl;
		};

		InfoLine info;
		PageLine page;
		std::unordered_map<int, CharLine> chars;

		// copy/move constructors
		//BMFontFile();
		//~BMFontFile();
		//BMFontFile(const BMFontFile& bmfile);
		//BMFontFile& operator=(const BMFontFile& rhs);
		//BMFontFile(BMFontFile&& bmfile);
		//BMFontFile& operator=(BMFontFile&& rhs);
	};

	void load(SDL_Renderer* sdl_renderer, const std::string& path);
	const std::unordered_map<int, MappedTexture>& getMap();
	const BMFontFile& getDefinition();
	void renderText(SDL_Renderer* sdl_renderer, const std::string& text, unsigned int x, unsigned int y);
}