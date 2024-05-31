#pragma once

#include <map>
#include <type_traits>
#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

namespace glp {
    namespace scene {
        struct Renderable {
            virtual void render() = 0;
            virtual ~Renderable() {};
        };

        struct Camera {
            glm::vec3 position      {0.0f, 1.0f, 3.0f};
            glm::vec3 front         {0.0f, 0.0f, -1.0f};
            glm::vec3 up            {0.0f, 1.0f, 0.0f};

            float yaw               {-90.0f};
            float pitch             {0.0f};
    
            float near              {0.1f};
            float far               {100.0f};
    
            float fov               {60.0f};
            float width             {};
            float height            {};
    
            void setup_dimensions(const glm::vec2& dimensions) {
                width = dimensions.x;
                height = dimensions.y;
            }
            virtual void calculate() = 0;
            virtual glm::mat4 view() const = 0;
            virtual glm::mat4 view_projection() const = 0;
            virtual ~Camera() {};
        };

        struct Controller {
            Camera* camera;

            virtual void handle_sdl_events(const std::vector<SDL_Event>& sdl_events) = 0;
            virtual void delta_time_update(float delta_time) = 0;
            virtual ~Controller() {};
        };
        
    }

    class Scene {
        protected:
            glm::vec2 screen_dimensions_;
            glm::vec3 background_color_;

            std::map<std::string, std::unique_ptr<scene::Renderable>> renderables_;
            std::map<std::string, std::unique_ptr<scene::Controller>> controllers_;
            std::map<std::string, std::unique_ptr<scene::Camera>> cameras_;

            scene::Controller* current_controller_ {nullptr};
            scene::Camera* current_camera_ {nullptr};

            std::function<void()> callback_ {nullptr};

        public:
            Scene(const glm::vec2& screen_dimensions);

            void loop(float delta_time, const std::vector<SDL_Event>& sdl_events);

            inline void set_background_color(const glm::vec3& color) { background_color_ = color; }
            inline void set_callback(const std::function<void()> callback) { callback_ = callback; }
            void set_controller(const std::string& name);
            void set_camera(const std::string& name);

            inline scene::Renderable* get_renderable(const std::string& name) { return renderables_.at(name).get(); }
            inline scene::Controller* get_controller(const std::string& name) { return controllers_.at(name).get(); }
            inline scene::Camera* get_camera(const std::string& name) { return cameras_.at(name).get(); }

            template<typename T>
            inline void add_camera(const std::string& name, std::unique_ptr<T>& object) {
                static_assert(std::is_base_of<scene::Camera, T>::value);
                object->setup_dimensions(screen_dimensions_);
                cameras_.insert_or_assign(name, std::move(object));
            }

            template<typename T>
            inline void add_renderable(const std::string& name, std::unique_ptr<T>& object) {
                static_assert(std::is_base_of<scene::Renderable, T>::value);
                renderables_.insert_or_assign(name, std::move(object));
            }

            template<typename T>
            inline void add_controller(const std::string& name, std::unique_ptr<T>& object) {
                static_assert(std::is_base_of<scene::Controller, T>::value);
                controllers_.insert_or_assign(name, std::move(object));
            }
    };
}
