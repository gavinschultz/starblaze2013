#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace debug
{
	class DebugItem
	{
	public:
		std::string label;
		std::string value;
	};

	void console(std::initializer_list<std::string> args);

	// TODO: templates - tried, but hard. Maybe see http://stackoverflow.com/questions/16591519/argument-deduction-with-template-member-function-and-non-template-overload
	//template<typename T>
	//auto debug::set<T>(std::string label, T value) -> decltype(std::to_string(value), void())
	//{
	//	items[label] = std::to_string(value);
	//}

	void set(std::string label, int value);
	void set(std::string label, unsigned int value);
	void set(std::string label, double value);
	void set(std::string label, std::string value);
	const std::unordered_map<std::string, std::string>& getItems();
}