#include "pch.h"
#include "ShaderUtils.h"
#include "ShaderParser.h"
#include <fstream>

void ShaderUtils::WriteShaderFiles(const std::string& outputDir, const std::string& shaderName, GLShaderParser::ShaderData data)
{
	std::string directory = outputDir;
	if (!outputDir.ends_with('/'))
		directory += "/";
	
	if (!data.vertexShader.empty())
	{
		WriteFile(directory + shaderName + ".vert", data.vertexShader);
	}
	if (!data.fragmentShader.empty())
	{
		WriteFile(directory + shaderName + ".frag", data.fragmentShader);
	}
}

std::string ShaderUtils::ToLowerCase(const std::string& inString)
{
	std::string lowercaseString;
	for (auto c : inString)
		lowercaseString += char(std::tolower(c));

	return lowercaseString;
}

void ShaderUtils::WriteFile(const std::string& fileName, const std::string& contents)
{
	std::ofstream file(fileName, std::ios::out);
	if (file.is_open())
	{
		file << contents;
		file.close();
		std::cout << "Wrote contents to file: " << fileName << std::endl;
	}
	else
	{
		std::cout << "Unable to open '" << fileName << "'" << std::endl;
	}
}
