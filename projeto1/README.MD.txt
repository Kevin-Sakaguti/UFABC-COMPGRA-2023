Kevin Sakaguti Porto - 11039416


Seja bem vindo ao programa projeto1 -> "Lousa Mágica".

O programa é desenvolvido em C++,se trata de uma "Lousa Mágica" que permite aos usuários criar 
desenhos e padrões sem que o "lápis saia do papel". Ele oferece a capacidade de criar padrões 
geométricos usando círculos e retas. Em essência, é uma aplicação de desenho digital que possibilita 
a criação de arte e geometria de forma interativa e virtual. Os usuários podem desenhar livremente 
ou criar padrões precisos com circulos, proporcionando uma plataforma versátil para expressão 
criativa e exploração matemática.


O projeto foi inspirado no "sierpinski", onde ele cria um fractal de triangulos com pontos "aleatorios".

O Projeto:

Diferente do sierpinski, ao invés de utilizar pontos calculados para a formação de um padrão geometrico.
No onPaintUI(), adicionei dois "Sliders" um na horizontal e um na vertical (SliderFloat e VSliderFloat, respectivamente).
Ambos os sliders são ajustados ao tamanho da tela automaticamente, utilizei a função 
m_viewportSize.x e m_viewportSize.y para adquirir o tamanho, em pixels, da tela. 
com o SetNextWindowPos e SetNextWindowSize, posicionei todas as janelas na posição e tamanho de acordo com as necessidades.

    // Window - Mexer na horizontal
    ImGui::SetNextWindowPos(ImVec2(175,0));
    ImGui::SetNextWindowSize(ImVec2(m_viewportSize.x-180-90, 70));
    ImGui::Begin("Mexer na Horizontal");
    ImGui::SetNextItemWidth(m_viewportSize.x-180-110);
    ImGui::SliderFloat(" ", &horizontalValue, 0.0f, 1.0f);
    ImGui::End();

    // Window - mexer na vertical
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x-85, 0));
    ImGui::SetNextWindowSize(ImVec2(85, m_viewportSize.y));
    ImGui::Begin("Vertical");
      ImGui::Text(" ");
      ImGui::SameLine();  // Coloca o slider na mesma linha que o texto
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);  // Apenas para arredondar as bordas do slider
      ImGui::VSliderFloat("##vslider", ImVec2(30, m_viewportSize.y - 50), &verticalValue, 0.0f, 1.0f);
      ImGui::PopStyleVar();
    ImGui::End();

Com estes atributos prontos, fiz a alteração no onPaint(), para mexer os pontos que estavam sendo criados. 
Adicionando a linha "m_P = glm::vec2((horizontalValue*2.0f)-1.0f,(verticalValue*2.0f)-1.0f);" 
O programa ja estava realizando a tarefa de pintar/desenhar na horizontal e na vertical.

Além de pintar o programa também faz circulos. e para isso temos alguma opções durante a execução do programa:
1) botão de desenhar os circulos
2) botao de fazer espiral
3) slider para escolher o raio do circulo

em onPaintUI:

    //Window - Construção do Circulo
    ImGui::SetNextWindowPos(ImVec2(5, 130));
    ImGui::SetNextWindowSize(ImVec2(175, 300));
    ImGui::Begin("Circulo");
      ImGui::Checkbox("Construir Circulo", &enabled);
      ImGui::Checkbox("Espiral", &enabled2);
      ImGui::SetNextItemWidth(170);
      ImGui::SliderFloat(" ", &r, 0.0f, 1.0f);
    ImGui::End(); 

    ImGui::SetNextWindowPos(ImVec2(5, 81));
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);
      if (ImGui::Button("Clear window", ImVec2(150, 30))) {
        abcg::glClear(GL_COLOR_BUFFER_BIT);
      }
    ImGui::End();

No programa acima, ele pega todos os atributos para a geração do circulo. Mas o calculo do circulo é feito no "onPaint":

if(enabled){
    float pi = M_PI;
    float x = cos(angle)*r;
    float y = sin(angle)*r;
    m_P = glm::vec2((horizontalValue*2.0f)-1.0f+x,(verticalValue*2.0f)-1.0f+y);

    if(angle<=2*pi){
      angle = angle + 0.01;
    }else{
      angle = 0.0f;
    }

    if(enabled2 and r > 0){
      r = r - 0.0001;
    } else if(enabled2 && r == 0){
      r = 0;
    }

Onde "enable" é a escolha de fazer o circulo ou não e o "enable2" é a opção de fazer a espiral.
para fazer o circulo, fiz ela com o calculo de triangulo reto, onde o raio do circulo é a hipotenusa do triangulo reto. 
Então a x = cos(angulo)/raio e y = sen(anglo)/raio

E como ultimo atributo coloquei a seleção de cor da linha/circulo a ser desenhado.

Em onPaintUI fiz a construção da caixa que seleciona a cor:

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

E para pegar a cor deste atributo, fiz uma variavel uniform vec4 no fragmentShader e deixei outColor igual a fragmentColor:

  auto const *fragmentShader{R"gl(#version 300 es
    precision mediump float;

    uniform vec4 fragmentColor;
    out vec4 outColor;

    void main() { outColor = fragmentColor; }
  )gl"};

e para fazer a alteração em fragmentColor acessei o fragmentShader com o seguinte comando:

  abcg::glUseProgram(m_program);

    glUniform4fv(glGetUniformLocation(m_program, "fragmentColor"), 1, &m_colors[0]);
    // Start using VAO
    abcg::glBindVertexArray(m_VAO);
    // Draw a single point
    abcg::glDrawArrays(GL_POINTS, 0, 1);
    // End using VAO
    abcg::glBindVertexArray(0);

  // End using the shader program
  abcg::glUseProgram(0);

Com a função "glUniform4fv" faço o acesso dos shaders, onde "m_program" é o vertex e fragment shader, faço o acesso da variavel "fragmentColor" e coloco um ponteiro para m_colors que é a variavel global que acessa o atributo de mudança de cor.


