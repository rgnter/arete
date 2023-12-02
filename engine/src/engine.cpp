#include "arete/engine.hpp"

namespace arete
{

ShaderHandle Engine::createShader(
  Shader::Stage stage,
  const std::vector<uint8_t>& source)
{
  auto handle = _shaderIndex++;
  _shaders.try_emplace(handle, stage, source);
  return handle;
}

Shader& Engine::getShader(ShaderHandle shaderHandle)
{
  return _shaders.at(shaderHandle);
}

}