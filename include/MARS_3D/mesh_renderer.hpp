#ifndef MARS_3D_MESH_RENDERER_
#define MARS_3D_MESH_RENDERER_

#include <MARS/engine/component.hpp>
#include <MARS/graphics/material.hpp>
#include <MARS/loaders/wavefront_loader.hpp>
#include <MARS/engine/layers/main_layers.hpp>
#include <MARS/math/matrix3.hpp>
#include <pl/safe_ptr.hpp>

namespace mars_3d {

    enum MARS_MESH_RENDER_ACTIVE_TYPE {
        MARS_MESH_RENDER_ACTIVE_TYPE_REF,
        MARS_MESH_RENDER_ACTIVE_TYPE_OBJ
    };

    struct mesh_group {
    private:
        MARS_MESH_RENDER_ACTIVE_TYPE type;

        mars_ref<mars_loader::wavefront_mesh> m_mesh_ref;
        mars_loader::mesh<mars_loader::wave_vertex> m_mesh;

        mars_ref<mars_graphics::material> m_mat;
        mars_ref<mars_graphics::shader_input> m_input;
        mars_ref<mars_graphics::graphics_engine> m_graphics;
        pl::safe_vector<mars_ref<mars_graphics::shader_data>> m_uniforms;

        std::atomic<bool> m_draw_executed = false;
    public:
        mesh_group(const std::string& _mat, const std::string& _mesh, const mars_ref<mars_graphics::graphics_engine>& _graphics);

        [[nodiscard]] inline const mars_loader::mesh<mars_loader::wave_vertex>& get_mesh() const { return type == MARS_MESH_RENDER_ACTIVE_TYPE_REF ? *m_mesh_ref.get().lock() : m_mesh; }

        inline void set_mesh(const mars_loader::mesh<mars_loader::wave_vertex>& _mesh) { m_mesh = _mesh; }

        [[nodiscard]] inline mars_ref<mars_graphics::material> get_material() const { return m_mat; }

        void add_uniform(const mars_ref<mars_graphics::shader_data>& _uniforms);

        void draw();

        void clear();
        bool destroy_uniform(const mars_ref<mars_graphics::shader_data>& _uniform);
        void destroy();
    };

    struct mesh_shader_mat {
        mars_math::matrix4<float> transform;
        mars_math::matrix4<float> model;
        mars_math::matrix3<float> normal;
    };

    class mesh_renderer : public mars_engine::component, public mars_layers::load_layer, public mars_layers::post_render_layer, public mars_layers::update_gpu {
    private:
        mars_loader::mesh<mars_loader::wave_vertex> m_mesh;
        mesh_shader_mat m_update_mat;
        std::shared_ptr<mesh_group> _group;
    public:
        mars_ref<mars_graphics::shader_data> uniforms;

        [[nodiscard]] inline const mars_loader::mesh<mars_loader::wave_vertex>& get_mesh() const { return _group->get_mesh(); }

        std::string mesh_path;
        std::string material_path;

        inline void set_material(const std::string& _path) { material_path = _path; }

        inline void set_mesh_path(const std::string& _path) { mesh_path = _path; }

        inline void set_mesh(const mars_loader::mesh<mars_loader::wave_vertex>& _mesh) {
            if (_group == nullptr)
                m_mesh = _mesh;
            else
                _group->set_mesh(_mesh);
        }

        void load() override;
        void send_to_gpu() override;
        void post_render() override;
        void destroy() override;
    };
}

#endif