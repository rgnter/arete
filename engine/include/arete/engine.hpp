#ifndef ARETE_ENGINE_HPP
#define ARETE_ENGINE_HPP

#include "core.hpp"

#include <span>
#include <array>
#include <vector>
#include <functional>
#include <unordered_map>

namespace arete
{

//! VulkanEngine.
class Engine {

  template<typename Key, typename Value>
  using Map = std::unordered_map<Key, Value>;

public:
  //! Creates shader.
  //! @param stage Shader stage.
  //! @param source Shader source.
  //! @returns Unique shader handle.
  ShaderHandle createShader(
    Shader::Stage stage,
    const std::vector<uint8_t>& source);

  //! Get shader.
  //! @param shaderHandle Shader handle.
  //! @returns Shader reference.
  Shader& getShader(ShaderHandle shaderHandle);

  //! Creates material.
  //! @param vertexShader Vertex shader handle.
  //! @param fragmentShader Fragment shader handle.
  //! @returns Unique material handle.
  MaterialHandle createMaterial(
    ShaderHandle vertexShader,
    ShaderHandle fragmentShader);

  //! Get material.
  //! @param materialHandle Material handle.
  //! @returns Material reference.
  Material& getMaterial(MaterialHandle materialHandle);

  //! Creates mesh.
  //! @param material Material handle.
  //! @param vertices Vertices.
  //! @param indices Indices.
  //! @returns Unique material handle.
  MeshHandle createMesh(
    MaterialHandle material,
    const Mesh::Vertices& vertices,
    const Mesh::Indices& indices);

  //! Get mesh.
  //! @param meshHandle Mesh handle.
  //! @returns Mesh reference.
  Mesh& getMesh(MeshHandle meshHandle);

  //! @return Shaders
  const Map<ShaderHandle, Shader>& shaders() const {
    return _shaders;
  }

  virtual void run() = 0;

private:
  ShaderHandle _shaderIndex { 0 };
  Map<ShaderHandle, Shader> _shaders;

  MaterialHandle _materialIndex { 0 };
  Map<MaterialHandle, Material> _materials;

  MeshHandle _meshIndex { 0 };
  Map<MeshHandle, Mesh> _meshes;
};

} // namespace arete

#endif//ARETE_ENGINE_HPP
