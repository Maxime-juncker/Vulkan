#pragma once

#include "Engine.h"
#include <string>
#include <vector>

namespace Application
{
	struct PiplineConfigInfo
	{

	};
	class Pipline
	{
	public:
		Pipline(const std::string& vertFilePath, const std::string& fragFilePath);

	private:
		static std::vector<char> ReadFile(const std::string& path);

		void CreateGraphicPipline(const std::string& vertFilePath, const std::string& fragFilePath);
	};
}