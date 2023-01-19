#ifndef MARS_3D_MESH_RENDERER_
#define MARS_3D_MESH_RENDERER_

#include <MARS/engine/component.hpp>
#include <MARS/graphics/material.hpp>
#include <MARS/loaders/wavefront_loader.hpp>
#include <MARS/engine/layers/main_layers.hpp>
#include <MARS/math/matrix3.hpp>

namespace mars_3d {

    struct mesh_group {
    private:
        mars_loader::wavefront_mesh* m_mesh = nullptr;
        mars_graphics::material* m_mat = nullptr;
        mars_graphics::shader_input* m_input = nullptr;
        mars_graphics::graphics_instance* m_instance = nullptr;
        pl::safe_vector<mars_graphics::shader_data*> m_uniforms;

        std::atomic<bool> m_draw_executed = false;
    public:
        mesh_group(const std::string& _mat, const std::string& _mesh, mars_graphics::graphics_instance* _instance);

        inline mars_graphics::material* get_material() { return m_mat; }

        void add_uniform(mars_graphics::shader_data* _uniforms);

        void draw();

        void clear();
        bool destroy_uniform(mars_graphics::shader_data* _uniform);
    };

    struct mesh_shader_mat {
        mars_math::matrix4<float> transform;
        mars_math::matrix4<float> model;
        mars_math::matrix3<float> normal;
    };

    class mesh_renderer : public mars_engine::component, public mars_layers::load_layer, public mars_layers::update_layer, public mars_layers::render_update_layer, public mars_layers::render_layer  {
    private:
        mesh_shader_mat m_update_mat;
        mesh_shader_mat m_rendering_mat;
        mesh_group* _group;
    public:
        mars_graphics::shader_data* uniforms = nullptr;

        std::string mesh_path;
        std::string material_path;

        inline void set_material(const std::string& _path) { material_path = _path; }
        inline void set_mesh_path(const std::string& _path) { mesh_path = _path; }

        void load() override;
        void prepare_gpu() override;
        void send_to_gpu() override;
        void post_render() override;
        void destroy() override;
    };
}

#endif