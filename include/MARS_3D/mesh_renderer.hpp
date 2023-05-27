#ifndef MARS_3D_MESH_RENDERER_
#define MARS_3D_MESH_RENDERER_

#include <MARS/engine/component.hpp>
#include <MARS/graphics/material.hpp>
#include <MARS/loaders/wavefront_loader.hpp>
#include <MARS/engine/layers/main_layers.hpp>
#include <MARS/math/matrix3.hpp>
#include <pl/safe.hpp>

namespace mars_3d {

    class mesh_handler {
    private:
        mars_ref<mars_graphics::material> m_mat;
        std::shared_ptr<mars_graphics::shader_input> m_input;
        mars_ref<mars_graphics::graphics_engine> m_graphics;
        pl::safe<std::vector<std::shared_ptr<mars_graphics::shader_data>>> m_uniforms;

        mars_ref<mars_loader::mesh<mars_loader::wave_vertex>> m_mesh;

        std::atomic<bool> m_bind_executed = false;
        std::atomic<bool> m_draw_executed = false;
    public:
        inline void set_mesh(const mars_ref<mars_loader::mesh<mars_loader::wave_vertex>>& _mesh) { m_mesh = _mesh; }

        inline void set_material(const mars_ref<mars_graphics::material>& _mat) { m_mat = _mat; }

        inline void set_graphics(const mars_ref<mars_graphics::graphics_engine>& _graphics) {
            m_graphics = _graphics;
        }

        inline std::shared_ptr<mars_graphics::shader_data> create_uniform() {
            auto uni = m_mat->generate_shader_data();
            m_uniforms.lock()->push_back(uni);
            return uni;
        }

        void create();

        void bind();
        void draw();

        inline void clear() {
            m_bind_executed = false;
            m_draw_executed = false;
        }

        void destroy();
    };

    struct mesh_shader_mat {
        mars_math::matrix4<float> transform;
        mars_math::matrix4<float> model;
        mars_math::matrix3<float> normal;
    };

    class mesh_renderer : public mars_engine::component, public mars_layers::load_layer, public mars_layers::post_render_layer, public mars_layers::update_gpu {
    private:
        mesh_shader_mat m_update_mat;
        std::shared_ptr<mesh_handler> m_mesh;
    public:
        std::shared_ptr<mars_graphics::shader_data> uniforms;

        void set_material(const std::string& _path);

        void set_mesh_path(const std::string& _path);

        inline void set_mesh(const mars_ref<mars_loader::mesh<mars_loader::wave_vertex>>& _mesh) {
            m_mesh->set_mesh(_mesh);
        }

        mesh_renderer();

        void load() override;
        void send_to_gpu() override;
        void post_render() override;
        void destroy() override;
    };
}

#endif