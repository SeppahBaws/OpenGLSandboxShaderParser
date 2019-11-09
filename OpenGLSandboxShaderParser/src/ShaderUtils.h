#pragma once

namespace GLShaderParser {
	struct ShaderData;
}

class ShaderUtils
{
public:
	static void WriteShaderFiles(const std::string& outputDir, const std::string& shaderName, GLShaderParser::ShaderData data);

private:
	static void WriteFile(const std::string& fileName, const std::string& contents);
};
