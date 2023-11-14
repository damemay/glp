#include "fonts.hh"
#include "utils.hh"

Font::Font(const std::string& path, const size_t& screen_w, const size_t& screen_h)
    : screen_width{screen_w}, screen_height{screen_h} {
    texture = new Texture{path};

#ifndef __vita__
    shader = new Shader{"../res/shaders/text.vert", "../res/shaders/text.frag"};
#else
    shader = new Shader{"../res/shaders/vita/vita_text.vert", "../res/shaders/vita/vita_text.frag"};
#endif
}

Font::~Font() {
    delete texture;
    delete shader;
}

Text::Text(Font* font_) : font{font_} {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void Text::update(const std::string& text, uint8_t size, uint16_t x, uint16_t y) {
    content = text;
    std::vector<glm::vec2> verts;
    for(size_t i=0; i<content.size(); i++) {
        char character = content.data()[i];

        glm::vec2 v_up_left     {x+i*(size/2), y+size};
        v_up_left -= glm::vec2(font->screen_width/2, font->screen_height/2);
        v_up_left /= glm::vec2(font->screen_width/2, font->screen_height/2);
        glm::vec2 v_up_right    {x+i*(size/2)+size, y+size};
        v_up_right -= glm::vec2(font->screen_width/2, font->screen_height/2);
        v_up_right /= glm::vec2(font->screen_width/2, font->screen_height/2);
        glm::vec2 v_down_right  {x+i*(size/2)+size, y};
        v_down_right -= glm::vec2(font->screen_width/2, font->screen_height/2);
        v_down_right /= glm::vec2(font->screen_width/2, font->screen_height/2);
        glm::vec2 v_down_left   {x+i*(size/2), y};
        v_down_left -= glm::vec2(font->screen_width/2, font->screen_height/2);
        v_down_left /= glm::vec2(font->screen_width/2, font->screen_height/2);

        float x = (character%16)/16.0f;
        float y = (character/16)/16.0f;
        glm::vec2 uv_up_left    {x, y};
        glm::vec2 uv_up_right   {x+1.0f/16.0f, y};
        glm::vec2 uv_down_right {x+1.0f/16.0f, y+1.0f/16.0f};
        glm::vec2 uv_down_left  {x, y+1.0f/16.0f};

        verts.push_back(v_up_left);
        verts.push_back(uv_up_left);
        verts.push_back(v_down_left);
        verts.push_back(uv_down_left);
        verts.push_back(v_up_right);
        verts.push_back(uv_up_right);
        verts.push_back(v_down_right);
        verts.push_back(uv_down_right);
        verts.push_back(v_up_right);
        verts.push_back(uv_up_right);
        verts.push_back(v_down_left);
        verts.push_back(uv_down_left);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(glm::vec2), verts.data(), GL_STATIC_DRAW);
#ifndef __vita__
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec2), (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(1);
#else
    font->shader->bind();
    GLuint id = glGetAttribLocation(shader->get(), "position");
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(id);
    id = glGetAttribLocation(shader->get(), "uv_in");
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec2), (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(id);

#endif
}

void Text::render() {
    glDisable(GL_DEPTH_TEST);
    font->shader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->texture->id);
    glBindVertexArray(VAO);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, content.size()*6);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

Text::~Text() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}