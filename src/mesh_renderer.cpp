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
    m_input = m_graphics->create<shader_input>();

    m_mat->set_pipeline<vertex3>();
    m_mat->get_pipeline()->set_viewport({ 0, 0 }, {1920, 1080 }, {0, 1 });

    m_input->create();
    m_input->bind();

    auto vertex = m_input->add_buffer(m_mesh->vertices.size() * sizeof(wave_vertex), MARS_MEMORY_TYPE_VERTEX);
    vertex->copy_data(m_mesh->vertices.data());

    auto index = m_input->add_buffer(m_mesh->indices.size() * sizeof(uint32_t), MARS_MEMORY_TYPE_INDEX);
    index->copy_data(m_mesh->indices.data());

    m_input->load_input(vertex3::get_description());

    m_input->unbind();
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

    m_uniforms.lock();

    for (auto& uni : m_uniforms) {
        uni->bind();
        m_graphics->primary_buffer()->draw_indexed(m_mesh->indices.size());
    }

    m_uniforms.unlock();
}

void mesh_handler::destroy() {
    m_input->destroy();
    for (const auto &uni : m_uniforms)
        uni->destroy();
    m_uniforms.clear();
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
    if (!uniforms.is_alive())
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
    m_mesh->destroy();
}