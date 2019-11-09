#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <ShaderParser.h>
#include <ShaderUtils.h>
using namespace GLShaderParser;

TEST_CASE("Simple shader gets split")
{
	ShaderParser parser;
	ShaderData data;
	bool success = parser.Parse("test/simpleSplit.glshader", data);

	REQUIRE(success);
	REQUIRE(!data.vertexShader.empty());
	REQUIRE(!data.fragmentShader.empty());
	REQUIRE(data.parameters.empty());
	ShaderUtils::WriteShaderFiles("test/output", "simpleSplit", data);
}

TEST_CASE("Parsing non-existing file fails")
{
	ShaderParser parser;
	ShaderData data;
	bool success = parser.Parse("test/non-existing.glshader", data);

	REQUIRE(!success);
}

TEST_CASE("Parameters get successfully read")
{
	ShaderParser parser;
	ShaderData data;
	bool success = parser.Parse("test/parameters.glshader", data);

	REQUIRE(success);
	REQUIRE(!data.vertexShader.empty());
	REQUIRE(!data.fragmentShader.empty());
	REQUIRE(!data.parameters.empty());

	for (Parameter parameter : data.parameters)
	{
		if (parameter.name == "u_Color")
			REQUIRE(parameter.type == DataType::Float3);
		if (parameter.name == "u_Ambient")
			REQUIRE(parameter.type == DataType::Float);
		if (parameter.name == "u_SunPos")
			REQUIRE(parameter.type == DataType::Float3);
	}

	ShaderUtils::WriteShaderFiles("test/output", "parameters", data);
}
