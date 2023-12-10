#ifndef ARETE_SHADER_HPP
#define ARETE_SHADER_HPP

#include "structures_common.hpp"

namespace arete
{
	
  //! Shader.
  class Shader {
  public:
    //! Shader stage.
    enum class Stage {
      Vertex, Fragment
    };

    //! Constructs shader with source which is bound to specified stage.
    //! @param stage Shader stage in the pipeline.
    //! @param source Shader source.
    explicit Shader(
      const Stage stage,
      std::vector<uint8_t> source) noexcept
      : _stage(stage)
      , _source(std::move(source))
    {}

    //! @returns Stage to which this shader is bound.
    [[nodiscard]] const Stage& stage() const
    {
      return _stage;
    }

    //! @returns Source of this shader.
    [[nodiscard]] const std::vector<uint8_t>& source() const
    {
      return _source;
    }

  private:
    //! Stage.
    Stage _stage;
    //! Source.
    std::vector<uint8_t> _source;
  };

}

#endif // ARETE_SHADER_HPP