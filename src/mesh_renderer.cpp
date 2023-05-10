#include <MARS_3D/mesh_renderer.hpp>
#include <MARS/resources/resource_manager.hpp>
#include <MARS/graphics/attribute/vertex3.hpp>
#include <MARS/math/vector4.hpp>

using namespace mars_resources;
using namespace mars_graphics;
using namespace mars_math;
using namespace mars_loader;
using namespace mars_3d;

void mesh_handler::create() {

    m_mat->set_pipeline<vertex3>();
    m_mat->get_pipeline().get().lock()->set_viewport({ 0, 0 }, {1920, 1080 }, {0, 1 });

    auto builder = m_graphics->builder<shader_input_builder>();

    auto vertex = builder.add_buffer(m_mesh->vertices.size() * sizeof(wave_vertex), MARS_MEMORY_TYPE_VERTEX);
    vertex->update(m_mesh->vertices.data());
    vertex->copy_data(0);

    auto index = builder.add_buffer(m_mesh->indices.size() * sizeof(uint32_t), MARS_MEMORY_TYPE_INDEX);
    index->update(m_mesh->indices.data());
    index->copy_data(0);

    builder.load_input(vertex3::get_description());

    m_input = builder.build();

    vertex->unbind();
    index->unbind();
}

void mesh_handler::bind() {
    if (m_bind_executed.exchange(true))
        return;

    m_mat->get_pipeline()->bind();
    m_mat->bind();
    m_input->bind();
}

void mesh_handler::draw() {
    if (m_draw_executed.exchange(true))
        return;

    for (auto& uni : *m_uniforms.lock().get()) {
        uni->bind();
        m_graphics->primary_buffer()->draw_indexed(m_mesh->indices.size());
    }
}

void mesh_handler::destroy() {
    m_input.reset();
    m_uniforms.lock()->clear();
}

mesh_renderer::mesh_renderer() {
    m_mesh = std::make_shared<mesh_handler>();
}

void mesh_renderer::set_material(const std::string& _path) {
    mars_ref<mars_graphics::material> mat;
    engine()->resources()->load_resource(_path, mat, graphics());
    m_mesh->set_material(mat);
}

void mesh_renderer::set_mesh_path(const std::string &_path) {
    mars_ref<mars_loader::wavefront_mesh> mesh;
    engine()->resources()->load_resource(_path, mesh);
    m_mesh->set_mesh(mesh.cast_static<mars_loader::mesh<wave_vertex>>());
}

void mesh_renderer::load() {
    m_mesh->set_graphics(graphics());
    m_mesh->create();
    uniforms = m_mesh->create_uniform();
    uniforms->update("position", &m_update_mat);


    graphics()->add_drawcall([&]() { m_mesh->bind(); m_mesh->draw(); });
}

void mesh_renderer::send_to_gpu() {
    if (!uniforms)
        return;

    m_update_mat.transform = graphics()->get_camera().get_proj_view() * transform().matrix();
    m_update_mat.model = transform().matrix();
    m_update_mat.normal = matrix3<float>(transform().matrix()).inverse().transpose();
    uniforms->get_uniform("position")->copy_data(graphics()->current_frame());
}

void mesh_renderer::post_render() {
    if (m_mesh == nullptr)
        return;

    m_mesh->clear();
}

void mesh_renderer::destroy() {
    uniforms.reset();
    m_mesh->destroy();
}