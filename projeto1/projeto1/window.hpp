#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <glm/fwd.hpp>
#include <random>

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

public:
  float verticalValue = 0.5f;
  float horizontalValue{0.5f};
  float angle = 0.0f;
  float r = 0.5f;
  bool enabled{false};
  bool enabled2{false};
  glm::vec4 m_colors{0.0f,0.0f,0.0f,1};
private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBOVertices{};
  GLuint m_program{};

  std::default_random_engine m_randomEngine;
  glm::vec2 m_P{};

  void setupModel();
};

#endif