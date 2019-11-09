#pragma once

#include <glad/glad.h>

namespace GLShaderParser
{
	enum class DataType : unsigned int
	{
		Bool		= GL_BOOL,
		Bool2		= GL_BOOL_VEC2,
		Bool3		= GL_BOOL_VEC3,
		Bool4		= GL_BOOL_VEC4,
		Int			= GL_INT,
		Int2		= GL_INT_VEC2,
		Int3		= GL_INT_VEC3,
		Int4		= GL_INT_VEC4,
		UInt		= GL_UNSIGNED_INT,
		UInt2		= GL_UNSIGNED_INT_VEC2,
		UInt3		= GL_UNSIGNED_INT_VEC3,
		UInt4		= GL_UNSIGNED_INT_VEC4,
		Float		= GL_FLOAT,
		Float2		= GL_FLOAT_VEC2,
		Float3		= GL_FLOAT_VEC3,
		Float4		= GL_FLOAT_VEC4,
		Double		= GL_DOUBLE,
		Double2		= GL_DOUBLE_VEC2,
		Double3		= GL_DOUBLE_VEC3,
		Double4		= GL_DOUBLE_VEC4,
		Mat3		= GL_FLOAT_MAT3,
		Mat4		= GL_FLOAT_MAT4,
		Sampler2D	= GL_SAMPLER_2D
	};

	struct ShaderConfig
	{
		std::string version;
	};

	struct Pragma
	{
		int lineNumber;
		std::string contents;
	};

	struct Region
	{
		std::string type = "";
		int startLine = -1;
		int endLine = -1;
	};

	struct Parameter
	{
		DataType type;
		std::string name;
		std::string rawLine;
	};

	struct Shader
	{
		std::string type = "";
		int startLine = -1;
		int endLine = -1;

		std::string code = "";
	};

	enum class ShaderType
	{
		Invalid,
		Vertex,
		Fragment,
		Geometry
	};

	enum class ParserState
	{
		None,
		Parameters,
		Varyings,
		VertexShader,
		FragmentShader
	};

	struct ShaderData
	{
		std::string vertexShader;
		std::string fragmentShader;

		std::vector<Parameter> parameters;
	};
	
	class ShaderParser
	{
	public:
		bool Parse(const std::string& filePath, ShaderData& data);

	private:
		bool ReadFile(const std::string& path, std::string& fileContents);
		ShaderType ShaderTypeFromString(const std::string& typeSrc) const;

		bool Analyze(const std::string& rawSource);
		bool Process();
		bool ProcessPragmas();
		bool ProcessParameters();
		bool ProcessShaders();

		bool ParseUniform(const std::string& rawLine, Parameter& param);
	
		std::map<ShaderType, std::string> Generate();

	private:
		ShaderConfig m_ShaderConfig;

		ParserState m_State = ParserState::None;

		std::map<int, std::string> m_Source;
		std::vector<Pragma> m_Pragmas;
		std::vector<Region> m_Regions;

		std::vector<Parameter> m_Parameters;
		std::map<ShaderType, Shader> m_Shaders;
		int m_ShaderLineCount = -1;

		static std::map<std::string, DataType> s_DataTypeLookup;
	};
}
