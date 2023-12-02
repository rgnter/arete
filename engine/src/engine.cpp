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

Shader& Engine::getShader(
  ShaderHandle shaderHandle)
{
  return _shaders.at(shaderHandle);
}

MaterialHandle Engine::createMaterial(
  ShaderHandle vertexShader,
  ShaderHandle fragmentShader)
{
  auto handle = _materialIndex++;
  _materials.try_emplace(handle, vertexShader, fragmentShader);
  return handle;
}

Material& Engine::getMaterial(
  MaterialHandle materialHandle)
{
  return _materials.at(materialHandle);
}

MeshHandle Engine::createMesh(
  MaterialHandle material,
  const Mesh::Vertices& vertices,
  const Mesh::Indices& indices)
{
  auto handle = _meshIndex++;
  _meshes.try_emplace(handle, material, vertices, indices);
  return handle;
}
Mesh& Engine::getMesh(MeshHandle meshHandle)
{
  return _meshes.at(meshHandle);
}

}// namespace arete