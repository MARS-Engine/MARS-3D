#ifndef MARS_3D_MESH_RENDERER_
#define MARS_3D_MESH_RENDERER_

#include <MARS/engine/component.hpp>
#include <MARS/graphics/material.hpp>
#include <MARS/loaders/wavefront_loader.hpp>
#include <MARS/engine/layers/main_layers.hpp>
#include <MARS/math/matrix3.hpp>
#include <pl/safe_ptr.hpp>

namespace mars_3d {

    struct mesh_group {
    private:
        std::shared_ptr<mars_loader::wavefront_mesh> m_mesh = nullptr;
        std::shared_ptr<mars_graphics::material> m_mat = nullptr;
        std::shared_ptr<mars_graphics::shader_input> m_input;
        mars_graphics::graphics_engine m_graphics;
        pl::safe_vector<std::shared_ptr<mars_graphics::shader_data>> m_uniforms;

        std::atomic<bool> m_draw_executed = false;
    public:
        mesh_group(const std::string& _mat, const std::string& _mesh, const mars_graphics::graphics_engine& _graphics);

        [[nodiscard]] inline std::shared_ptr<mars_loader::wavefront_mesh> get_mesh() const { return m_mesh; }
        [[nodiscard]] inline std::shared_ptr<mars_graphics::material> get_material() const { return m_mat; }

        void add_uniform(const std::shared_ptr<mars_graphics::shader_data>& _uniforms);

        void draw();

        void clear();
        bool destroy_uniform(const std::shared_ptr<mars_graphics::shader_data>& _uniform);
        void destroy();
    };

    struct mesh_shader_mat {
        mars_math::matrix4<float> transform;
        mars_math::matrix4<float> model;
        mars_math::matrix3<float> normal;
    };

    class mesh_renderer : public mars_engine::component, public mars_layers::load_layer, public mars_layers::update_layer, public mars_layers::post_render_layer, public mars_layers::update_gpu {
    private:
        mesh_shader_mat m_update_mat;
        mesh_group* _group;
    public:
        std::shared_ptr<mars_graphics::shader_data> uniforms = nullptr;

        [[nodiscard]] inline std::shared_ptr<mars_loader::wavefront_mesh> get_mesh() const { return _group->get_mesh(); }

        std::string mesh_path;
        std::string material_path;

        inline void set_material(const std::string& _path) { material_path = _path; }
        inline void set_mesh_path(const std::string& _path) { mesh_path = _path; }

        void load() override;
        void send_to_gpu() override;
        void post_render() override;
        void destroy() override;
    };
}

#endif