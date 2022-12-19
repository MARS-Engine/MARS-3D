#ifndef TEST_COMPONENT_
#define TEST_COMPONENT_

#include <MVRE/engine/component.hpp>
#include <MVRE/graphics/material.hpp>
#include <MVRE/loaders/wavefront_loader.hpp>
#include <MVRE/engine/layers/main_layers.hpp>

namespace mvre_3d {

    class mesh_renderer : public mvre_engine::component<mesh_renderer>, public mvre_layers::load_layer, public mvre_layers::update_layer, public mvre_layers::render_layer  {
        public:
        mvre_loader::wavefront_mesh* mesh;
        mvre_graphics::shader_input* input = nullptr;
        mvre_graphics::shader_data* uniforms;

        std::string mesh_path;
        std::string material_path;

        inline void set_material(const std::string& _path) { material_path = _path; }
        inline void set_mesh_path(const std::string& _path) { mesh_path = _path; }

        void load() override;
        void update() override;
        void destroy() override;
    };
}

#endif