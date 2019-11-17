#pragma once

namespace GLShaderParser {
	struct ShaderData;
}

#ifdef _DEBUG
	#define SHADER_LOG(msg) std::cout << "[GLShaderParser] " << msg << std::endl
	#define SHADER_LOG_ERR(msg) std::cout << "[GLShaderParser] |ERROR| " << msg << std::endl
#else
	#define SHADER_LOG(msg)
	#define SHADER_LOG_ERR(msg)
#endif

class ShaderUtils
{
public:
	static void WriteShaderFiles(const std::string& outputDir, const std::string& shaderName, GLShaderParser::ShaderData data);

	static std::string ToLowerCase(const std::string& inString);

private:
	static void WriteFile(const std::string& fileName, const std::string& contents);
};
