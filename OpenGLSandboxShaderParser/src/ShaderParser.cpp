#include "pch.h"
#include "ShaderParser.h"
#include <algorithm>
#include <locale>

namespace GLShaderParser
{
	std::map<std::string, DataType> ShaderParser::s_DataTypeLookup = {
			{"bool",		DataType::Bool},
			{"bvec2",		DataType::Bool2},
			{"bvec3",		DataType::Bool3},
			{"bvec4",		DataType::Bool4},
			{"int",			DataType::Int},
			{"ivec2",		DataType::Int2},
			{"ivec3",		DataType::Int3},
			{"ivec4",		DataType::Int4},
			{"uint",		DataType::UInt},
			{"uvec2",		DataType::UInt2},
			{"uvec3",		DataType::UInt3},
			{"uvec4",		DataType::UInt4},
			{"float",		DataType::Float},
			{"vec2",		DataType::Float2},
			{"vec3",		DataType::Float3},
			{"vec4",		DataType::Float4},
			{"double",		DataType::Double},
			{"dvec2",		DataType::Double2},
			{"dvec3",		DataType::Double3},
			{"dvec4",		DataType::Double4},
			{"mat3",		DataType::Mat3},
			{"mat4",		DataType::Mat4},
			{"sampler2D",	DataType::Sampler2D}
	};

	bool ShaderParser::Parse(const std::string& filePath, ShaderData& data)
	{
		std::string shaderSource;
		if (!ReadFile(filePath, shaderSource))
			return false;

		if (!Analyze(shaderSource))
			return false;
		if (!Process())
			return false;

		const std::map<ShaderType, std::string> sources = Generate();

		std::cout << "Parsing Done!" << std::endl;

		for (auto it = sources.begin(); it != sources.end(); ++it)
		{
			switch (it->first)
			{
			case ShaderType::Vertex:
				data.vertexShader = it->second;
				break;
			case ShaderType::Fragment:
				data.fragmentShader = it->second;
				break;
			default:
				return false;
			}
		}

		data.parameters = m_Parameters;

		return true;
	}

	bool ShaderParser::ReadFile(const std::string& path, std::string& fileContents)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (!in)
		{
			std::cout << "Unable to open file '" << path << "'" << std::endl;
			return false;
		}

		// Write the contents of the file into fileContents
		in.seekg(0, std::ios::end);
		fileContents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&fileContents[0], fileContents.size());
		in.close();

		return true;
	}

	ShaderType ShaderParser::ShaderTypeFromString(const std::string& typeSrc) const
	{
		ShaderType type = ShaderType::Invalid;

		std::string lowerType;
		for (auto c : typeSrc)
			lowerType += std::tolower(c);

		if (lowerType == "vertex")
			type = ShaderType::Vertex;
		else if (lowerType == "fragment")
			type = ShaderType::Fragment;
		else if (lowerType == "geometry")
			type = ShaderType::Geometry;

		return type;
	}

	bool ShaderParser::Analyze(const std::string& rawSource)
	{
		const char* pragma = "#pragma";
		const char* region = "#region";
		const char* endregion = "#endregion";
		const char* shader = "#shader";
		const char* endshader = "#endshader";
		const size_t pragmaLength = strlen(pragma);
		const size_t regionLength = strlen(region);
		const size_t shaderLength = strlen(shader);

		Region currentRegion{ "", -1, -1 };
		Shader currentShader{ "", -1, -1 };

		std::istringstream s(rawSource);
		std::string line;
		int lineNr = 0;
		while (std::getline(s, line))
		{
			lineNr++;
			m_Source[lineNr] = line; // Store source code for later reference

			// If this line contains a comment, we want to only search in the part of the line
			// that isn't commented. Only single-line comments supported for now.
			size_t commentPos = line.find("//");
			if (commentPos != std::string::npos)
				line = line.substr(0, commentPos);

			// Check if we're in a #region
			size_t pos = line.find(region);
			if (pos != std::string::npos)
			{
				size_t eol = line.find_first_of("\r\n", pos);
				size_t begin = pos + regionLength + 1; // space after token
				std::string contents = line.substr(begin, eol - begin);

				if (contents == "parameters")
					m_State = ParserState::Parameters;
				else if (contents == "varyings")
					m_State = ParserState::Varyings;

				currentRegion.type = contents;
				currentRegion.startLine = lineNr;

				continue; // we can't have any of the other stuff on the same line.
			}

			if (m_State == ParserState::Parameters || m_State == ParserState::Varyings)
			{
				pos = line.find(endregion);
				if (pos != std::string::npos)
				{
					m_State = ParserState::None;
					currentRegion.endLine = lineNr;
					m_Regions.push_back(currentRegion);
					currentRegion = Region{ "", -1, -1 };
				
					continue;
				}
			}

			// Check if we're in a #shader
			pos = line.find(shader);
			if (pos != std::string::npos)
			{
				size_t eol = line.find_first_of("\r\n", pos);
				size_t begin = pos + shaderLength + 1; // space after token
				std::string contents = line.substr(begin, eol - begin);

				if (contents == "vertex")
					m_State = ParserState::VertexShader;
				else if (contents == "fragment")
					m_State = ParserState::FragmentShader;

				currentShader.type = contents;
				currentShader.startLine = lineNr;

				continue;
			}

			if (m_State == ParserState::VertexShader || m_State == ParserState::FragmentShader)
			{
				pos = line.find(endshader);
				if (pos != std::string::npos)
				{
					m_State = ParserState::None;
					currentShader.endLine = lineNr;

					ShaderType type = ShaderTypeFromString(currentShader.type);
					if (type == ShaderType::Invalid)
					{
						std::cerr << "Invalid shader type!" << std::endl;
						return false;
					}
					
					m_Shaders[type] = currentShader;
					currentShader = Shader{ "", -1, -1 };

					continue;
				}
			}

			// Analyze all #pragma
			pos = line.find(pragma);
			if (pos != std::string::npos)
			{
				size_t eol = line.find_first_of("\r\n", pos); // end of the line
				size_t begin = pos + pragmaLength + 1; // space after token
				std::string contents = line.substr(begin, eol - begin);

				m_Pragmas.push_back({ lineNr, contents });
			}
		}

		m_ShaderLineCount = lineNr;
		return true;
	}

	bool ShaderParser::Process()
	{
		if (!ProcessPragmas())
			return false;

		if (!ProcessParameters())
			std::cout << "This shader doesn't contain any material parameters." << std::endl;

		if (!ProcessShaders())
			return false;

		return true;
	}

	bool ShaderParser::ProcessPragmas()
	{
		for (auto& pragma : m_Pragmas)
		{
			if (pragma.contents.find("version") != std::string::npos)
			{
				m_ShaderConfig.version = pragma.contents;
			}
		}

		return true;
	}

	bool ShaderParser::ProcessParameters()
	{
		const auto it = std::find_if(m_Regions.begin(), m_Regions.end(), [](const Region& region)
		{
			return region.type == "parameters";
		});

		if (it == m_Regions.end()) return false;

		for (int i = it->startLine + 1; i < it->endLine; ++i)
		{
			if (m_Source[i].find("uniform") != std::string::npos)
			{
				Parameter param{};
				if (!ParseUniform(m_Source[i], param))
					return false;

				m_Parameters.push_back(param);
			}
		}

		return true;
	}

	bool ShaderParser::ProcessShaders()
	{
		std::string shaderSource;

		for (auto it = m_Shaders.begin(); it != m_Shaders.end(); ++it)
		{
			Shader& shader = it->second;
			shaderSource = "";

			// Shader start line contains "#shader <type>"
			// and Shader end line contains "#endshader",
			// so we need to start a line later and end a line earlier.
			for (int i = shader.startLine + 1; i < shader.endLine; i++)
			{
				shaderSource += m_Source[i];
			}

			shader.code += shaderSource + "\n";
		}

		std::cout << "Shaders processed!" << std::endl;
		return true;
	}

	bool ShaderParser::ParseUniform(const std::string& rawLine, Parameter& param)
	{
		// TODO: no default values yet.
		// const static std::regex r = std::regex("^\\s*uniform\\s+(\\w+)\\s+(\\w+)(?:\\s*=\\s*(.+?))?;.*$");
		const std::regex r = std::regex("\\s*uniform\\s+(\\w+)\\s+(\\w+)\\s*;.*\\r*");
		std::smatch matches;

		std::string type;

		if (std::regex_match(rawLine, matches, r))
		{
			if (matches.size() != 3)
			{
				std::cerr << "Invalid uniform composition." << std::endl;
				return false;
			}

			type = matches[1];
			param.type = s_DataTypeLookup[type];
			param.name = matches[2];
			param.rawLine = rawLine;

			return true; // successfully parsed uniform variable.
		}

		std::cerr << "No matches with uniforms." << std::endl;
		return false;
	}

	std::map<ShaderType, std::string> ShaderParser::Generate()
	{
		std::map<ShaderType, std::string> sources;

		std::string currentShader;
		for (auto it = m_Shaders.begin(); it != m_Shaders.end(); ++it)
		{
			Shader shader = it->second;
			currentShader = "";

			// Shader version
			currentShader = "#" + m_ShaderConfig.version + "\n";

			// Uniforms
			for (Parameter parameter : m_Parameters)
			{
				currentShader += parameter.rawLine;
			}
			currentShader += "\n";

			// Shader code
			currentShader += shader.code;

			ShaderType type;
			if (shader.type == "vertex")
				type = ShaderType::Vertex;
			else if (shader.type == "fragment")
				type = ShaderType::Fragment;

			sources[type] = currentShader;
		}

		return sources;
	}
	
}
