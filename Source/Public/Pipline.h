#pragma once

#include <string>
#include <vector>

namespace Application
{
	class Pipline
	{
	public:
		Pipline(const std::string& vertFilePath, const std::string& fragFilePath);

	private:
		static std::vector<char> ReadFile(const std::string& path);

		void CreateGraphicPipline(const std::string& vertFilePath, const std::string& fragFilePath);
	};
}