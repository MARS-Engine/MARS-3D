#ifndef __TEST__COMPONENT__
#define __TEST__COMPONENT__

#include <MVRE/engine/component.hpp>
#include <MVRE/graphics/material.hpp>
#include <MVRE/loaders/wavefront_loader.hpp>
#include <MVRE/engine/layers/main_layers.hpp>

namespace mvre_3d {

    class mesh_renderer : public mvre_engine::component<mesh_renderer>, public mvre_layers::load_layer, public mvre_layers::update_layer, public mvre_layers::render_layer  {
        public:
        mvre_loader::wavefront_mesh* mesh;
        mvre_graphics::shader_input* input = nullptr;
        mvre_graphics::pipeline* new_pipeline = nullptr;

        void load() override;
        void update() override;
    };
}

#endif