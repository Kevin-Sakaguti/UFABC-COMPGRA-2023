#include "window.hpp"
#include "abcgOpenGLFunction.hpp"
#include "imgui.h"
#include <glm/fwd.hpp>
#include <cmath>

void Window::onCreate() {
  auto const *vertexShader{R"gl(#version 300 es
    layout(location = 0) in vec2 inPosition;

    void main() { 
      gl_PointSize = 2.0;
      gl_Position = vec4(inPosition, 0, 1); 
    }
  )gl"};

  auto const *fragmentShader{R"gl(#version 300 es
    precision mediump float;

    uniform vec4 fragmentColor;
    out vec4 outColor;

    void main() { outColor = fragmentColor; }
  )gl"};

  // Create shader program
  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  // Clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  std::array<GLfloat, 2> sizes{};
#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
  abcg::glGetFloatv(GL_POINT_SIZE_RANGE, sizes.data());
#else
  abcg::glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, sizes.data());
#endif
  fmt::print("Point size: {:.2f} (min), {:.2f} (max)\n", sizes.at(0),
             sizes.at(1));

  // Start pseudorandom number generator
  auto const seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  // Randomly pick a pair of coordinates in the range [-1; 1)
  std::uniform_real_distribution<float> realDistribution(-1.0f, 1.0f);
  m_P.x = realDistribution(m_randomEngine);
  m_P.y = realDistribution(m_randomEngine);
}

void Window::onPaint() {
  // Create OpenGL buffers for drawing the point at m_P
  setupModel();
  // Set the viewport
  auto minSize{std::min(m_viewportSize.x,m_viewportSize.y)};
  auto teste_x = (m_viewportSize.x-minSize)/2;
  auto teste_y = (m_viewportSize.y-minSize)/2;
  abcg::glViewport(teste_x, teste_y, minSize, minSize);
  // Start using the shader program
  abcg::glUseProgram(m_program);

    // put the colors of the window color selector in the constant fragmentColor the is used on fragmentShader
    glUniform4fv(glGetUniformLocation(m_program, "fragmentColor"), 1, &m_colors[0]);
    // Start using VAO
    abcg::glBindVertexArray(m_VAO);
    // Draw a single point
    abcg::glDrawArrays(GL_POINTS, 0, 1);
    // End using VAO
    abcg::glBindVertexArray(0);

  // End using the shader program
  abcg::glUseProgram(0);


  //in case of enable (checkbox), the program starts making circles.
  if(enabled){
    float pi = M_PI;
    float x = cos(angle)*r;
    float y = sin(angle)*r;
    m_P = glm::vec2((horizontalValue*2.0f)-1.0f+x,(verticalValue*2.0f)-1.0f+y);

    if(angle<=2*pi){
      angle = angle + 0.008;
    }else{
      angle = 0.0f;
    }
    //in case of enable (checkbox), the program starts making spirals.
    if(enabled2 and r > 0){
      r = r - 0.0001;
    } else if(enabled2 && r == 0){
      r = 0;
    }
  
  //in case of enable = false, the program allow the user to draw vertical and horizontal lines
  } else {
    m_P = glm::vec2((horizontalValue*2.0f)-1.0f,(verticalValue*2.0f)-1.0f);
  }
    

  // Print coordinates to console
  // fmt::print("({:+.2f}, {:+.2f})\n", m_P.x, m_P.y);
  //


}

void Window::setupModel() {
  // Release previous VBO and VAO
  abcg::glDeleteBuffers(1, &m_VBOVertices);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Generate a new VBO and get the associated ID
  abcg::glGenBuffers(1, &m_VBOVertices);
  // Bind VBO in order to use it
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
  // Upload data to VBO
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_P), &m_P, GL_STATIC_DRAW);
  // Unbinding the VBO is allowed (data can be released now)
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  // Release shader program, VBO and VAO
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOVertices);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  // Our own ImGui widgets go below
  {
    // Window - horizontal movement
    ImGui::SetNextWindowPos(ImVec2(175,0));
    ImGui::SetNextWindowSize(ImVec2(m_viewportSize.x-180-90, 70));
    ImGui::Begin("Mexer na Horizontal");
    ImGui::SetNextItemWidth(m_viewportSize.x-180-110);
    ImGui::SliderFloat(" ", &horizontalValue, 0.0f, 1.0f);
    ImGui::End();

    // Window - vertical movement
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x-85, 0));
    ImGui::SetNextWindowSize(ImVec2(85, m_viewportSize.y));
    ImGui::Begin("Vertical");
      ImGui::Text(" ");
      ImGui::SameLine();  //Put the slider in the same line that the text is.
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);  // Apenas para arredondar as bordas do slider
      ImGui::VSliderFloat("##vslider", ImVec2(30, m_viewportSize.y - 50), &verticalValue, 0.0f, 1.0f);
      ImGui::PopStyleVar();
    ImGui::End();

    //Window - Construção do Circulo
    ImGui::SetNextWindowPos(ImVec2(5, 130));
    ImGui::SetNextWindowSize(ImVec2(175, 300));
    ImGui::Begin("Circulo");
      ImGui::Checkbox("Construir Circulo", &enabled); //Checkbox to draw a circle
      ImGui::Checkbox("Espiral", &enabled2); //Checkbox to draw a spiral
      ImGui::SetNextItemWidth(170); 
      ImGui::SliderFloat(" ", &r, 0.0f, 1.0f); // Slider to set the radius of the circle
    ImGui::End(); 

    //Make the window to clear the draw
    ImGui::SetNextWindowPos(ImVec2(5, 81));
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);
      if (ImGui::Button("Clear window", ImVec2(150, 30))) {
        abcg::glClear(GL_COLOR_BUFFER_BIT);
      }
    ImGui::End();

    //Make the window to set the color
    ImGui::SetNextWindowSize(ImVec2(200,80));
    ImGui::SetNextWindowPos(ImVec2(5, m_viewportSize.y - 100));
    ImGui::Begin("Cores");
      // Edit vertex colors
      auto colorEditFlags{ImGuiColorEditFlags_NoTooltip};
      ImGui::PushItemWidth(215);
      ImGui::ColorEdit3("Cor", &m_colors.x, colorEditFlags);
      ImGui::PopItemWidth();
    ImGui::End();
  }
}