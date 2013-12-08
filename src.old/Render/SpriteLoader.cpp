#include "stdafx.h"
#include <algorithm>
#include <regex>
#include <exception>
#include <rapidjson\rapidjson.h>
#include <rapidjson\filestream.h>
#include <rapidjson\reader.h>
#include <rapidjson\document.h>
#include "SpriteLoader.h"
#include "RenderUtil.h"
#include "FileLoader.h"
#include "Debug.h"

using namespace rapidjson;

/*
Loads a spritesheet with a standard TexturePacker-generated JSON descriptor in a format like that shown below:

{"frames": {

"alien1.tga":
{
"frame": {"x":32,"y":88,"w":16,"h":12},
"rotated": false,
"trimmed": false,
"spriteSourceSize": {"x":0,"y":0,"w":16,"h":12},
"sourceSize": {"w":16,"h":12}
},
"ship.tga":
{
"frame": {"x":95,"y":80,"w":32,"h":8},
"rotated": false,
"trimmed": false,
"spriteSourceSize": {"x":0,"y":0,"w":32,"h":8},
"sourceSize": {"w":32,"h":8}
}},
"meta": {
"app": "http://www.codeandweb.com/texturepacker",
"version": "1.0",
"image": "spritesheet.tga",
"format": "RGBA8888",
"size": {"w":128,"h":128},
"scale": "1",
"smartupdate": "$TexturePacker:SmartUpdate:491d1f666fb21402395bb0e9110794d6:6478106c968d1829ac20af9934c24d28:aade35f590a2fbcd5cdfd4ce8477e23b$"
}
}
*/

SpriteLoader::SpriteLoader()
{

}

void SpriteLoader::load(SDL_Renderer* sdl_renderer, const std::string& spritesheet_json_path)
{
	console_debug({ "Loading spritesheet from JSON in ", spritesheet_json_path });

	FileLoader file_loader{ spritesheet_json_path, "r" };
	FILE* file_spritesheet = file_loader();

	Reader reader;
	FileStream json_is{ file_spritesheet };

	Document json_doc;

	if (json_doc.ParseStream<0, FileStream>(json_is).HasParseError())
		throw std::runtime_error("The JSON in file " + spritesheet_json_path + " could not be parsed.");

	if (!json_doc.HasMember("meta"))
		throw std::runtime_error("The JSON in file " + spritesheet_json_path + " does not have a valid 'meta' field.");
	const auto& meta = json_doc["meta"];

	if (!meta.HasMember("image"))
		throw std::runtime_error("The JSON in file " + spritesheet_json_path + " does not have a valid 'meta/image' field.");
	const std::string spritesheet_image_filename = meta["image"].GetString();

	if (_texture)
	{
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}

	_texture = renderutil::loadTextureFromFile(sdl_renderer, "resources\\" + spritesheet_image_filename, &_texture_rect);

	if (!json_doc.HasMember("frames"))
		throw std::runtime_error("The JSON in file " + spritesheet_json_path + " does not have a valid 'frames' field.");
	const auto& frames = json_doc["frames"];

	for (Value::ConstMemberIterator it = frames.MemberBegin(); it != frames.MemberEnd(); ++it)
	{
		const std::string sprite_name = it->name.GetString();
		if (!it->value.HasMember("frame"))
			throw std::runtime_error("The JSON in file " + spritesheet_json_path + " does not have a valid 'frames/" + sprite_name + "/frame' field.");
		const auto& frame = it->value["frame"];

		if (!frame.HasMember("x"))
			throw std::runtime_error("The JSON in file " + spritesheet_json_path + " does not have a valid 'frames/" + sprite_name + "/frame/x' field.");
		if (!frame.HasMember("y"))
			throw std::runtime_error("The JSON in file " + spritesheet_json_path + " does not have a valid 'frames/" + sprite_name + "/frame/y' field.");
		if (!frame.HasMember("w"))
			throw std::runtime_error("The JSON in file " + spritesheet_json_path + " does not have a valid 'frames/" + sprite_name + "/frame/w' field.");
		if (!frame.HasMember("h"))
			throw std::runtime_error("The JSON in file " + spritesheet_json_path + " does not have a valid 'frames/" + sprite_name + "/frame/h' field.");

		const unsigned int frame_x = frame["x"].GetUint();
		const unsigned int frame_y = frame["y"].GetUint();
		const unsigned int frame_w = frame["w"].GetUint();
		const unsigned int frame_h = frame["h"].GetUint();

		std::regex pattern{ "(.*)(.tga)" };
		std::string replace{ "$1" };
		std::string sprite_name_parsed = std::regex_replace(sprite_name, pattern, replace);
		this->_sprites_by_name[sprite_name_parsed] = SpriteTexture{ _texture, SDL_Rect{ frame_x, frame_y, frame_w, frame_h } };

		console_debug({ "Loaded sprite ", sprite_name, " x:", std::to_string(frame_x), " y:", std::to_string(frame_y), " w:", std::to_string(frame_w), " h:", std::to_string(frame_h) });
	}

	console_debug({ "Loaded spritesheet '", spritesheet_image_filename, "'" });
}

const SpriteTexture& SpriteLoader::getSprite(const std::string& name)
{
	if (_sprites_by_name.count(name) == 0)
		throw std::runtime_error("The sprite " + name + " was not found.");

	return _sprites_by_name[name];
}