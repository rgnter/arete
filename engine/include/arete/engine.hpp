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

  //! @return Shaders
  const Map<ShaderHandle, Shader>& shaders() const {
    return _shaders;
  }

  virtual void run() = 0;

private:
  ShaderHandle _shaderIndex { 0 };
  Map<ShaderHandle, Shader> _shaders;

  Map<MaterialHandle, Material> _materials;
  Map<MeshHandle, Mesh> _meshes;
};

} // namespace arete

#endif//ARETE_ENGINE_HPP
