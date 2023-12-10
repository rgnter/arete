#ifndef ARETE_MATERIAL_HPP
#define ARETE_MATERIAL_HPP

#include "structures_common.hpp"

namespace arete
{

  //! Material.
  class Material
  {
  public:
    //! Constructs material with specified shaders.
    //! @param vertexShader Vertex shader.
    //! @param fragmentShader Fragment shader.
    explicit Material(
      const ShaderHandle vertexShader,
      const ShaderHandle fragmentShader) noexcept
        : _vertexShader(vertexShader)
        , _fragmentShader(fragmentShader)
    {}

    //! @returns Vertex shader handle.
    [[nodiscard]] ShaderHandle vertexShader() const
    {
      return _vertexShader;
    }

    //! @returns Fragment shader handle.
    [[nodiscard]] ShaderHandle fragmentShader() const
    {
      return _fragmentShader;
    }

  private:
    ShaderHandle _vertexShader;
    ShaderHandle _fragmentShader;
  };

}

#endif	// ARETE_MATERIAL_HPP