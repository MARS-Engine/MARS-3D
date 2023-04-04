#include <MARS_3D/mesh_renderer.hpp>
#include <MARS/resources/resource_manager.hpp>
#include <MARS/graphics/attribute/vertex3.hpp>
#include <MARS/math/vector4.hpp>

using namespace mars_resources;
using namespace mars_graphics;
using namespace mars_math;
using namespace mars_loader;
using namespace mars_3d;

pl::safe_map<std::pair<std::string, std::string>, mesh_group*> multi_meshes;

mesh_group* get_mesh_group(const std::string& _mat, const std::string& _mesh, const mars_graphics::graphics_engine& _graphics) {
    auto pair = std::make_pair(_mat, _mesh);

    multi_meshes.lock();
    if (multi_meshes.contains(pair)) {
        multi_meshes.unlock();
        return multi_meshes[pair];
    }

    auto group = new mesh_group(_mat, _mesh, _graphics);

    multi_meshes[pair] = group;
    multi_meshes.unlock();

    return group;
}

void destroy_group(const std::string& _mat, const std::string& _mesh, const mars_ref<shader_data>& _uniform) {
    auto pair = std::make_pair(_mat, _mesh);

    multi_meshes.lock();
    if (multi_meshes[pair]->destroy_uniform(_uniform)) {
        multi_meshes[pair]->destroy();
        delete multi_meshes[pair];
        multi_meshes.erase(pair);
    }

    if (multi_meshes.empty())
        multi_meshes.clear();

    multi_meshes.unlock();
}


mesh_group::mesh_group(const std::string& _mat, const std::string& _mesh, const mars_graphics::graphics_engine& _graphics) {
    m_graphics = _graphics;

    m_graphics->resources()->load_resource(_mesh, m_mesh);

    m_input = m_graphics->create<shader_input>();

    m_graphics->resources()->load_resource(_mat, m_mat, m_graphics);
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

void mesh_group::add_uniform(const mars_ref<mars_graphics::shader_data>& _uniforms) {
    m_uniforms.lock();
    m_uniforms.push_back(_uniforms);
    m_uniforms.unlock();
}

void mesh_group::draw() {
    if (m_draw_executed.exchange(true))
        return;

    m_mat->get_pipeline()->bind();

    std::map<size_t, texture*> active_textures;

    m_mat->bind();
    m_input->bind();

    m_uniforms.lock();

    for (auto& uni : m_uniforms) {
        uni->bind();
        m_graphics->primary_buffer()->draw_indexed(m_mesh->indices.size());
    }

    m_uniforms.unlock();
}

void mesh_group::clear() {
    m_draw_executed.exchange(false);
}

bool mesh_group::destroy_uniform(const mars_ref<mars_graphics::shader_data>& _uniform) {
    m_uniforms.lock();

    m_uniforms.erase(std::find(m_uniforms.begin(), m_uniforms.end(), _uniform));
    _uniform->destroy();

    auto is_empty  = m_uniforms.empty();
    m_uniforms.unlock();
    return is_empty;
}

void mesh_group::destroy() {
    m_input->destroy();
}

void mesh_renderer::load() {
    _group = get_mesh_group(material_path, mesh_path, graphics());

    uniforms = _group->get_material()->generate_shader_data();

    _group->add_uniform(uniforms);

    uniforms->update("position", &m_update_mat);

    graphics()->add_drawcall([&]() { _group->draw(); });
}

void mesh_renderer::send_to_gpu() {
    m_update_mat.transform = graphics()->get_camera().get_proj_view() * transform().matrix();
    m_update_mat.model = transform().matrix();
    m_update_mat.normal = matrix3<float>(transform().matrix()).inverse().transpose();
    uniforms->get_uniform("position")->copy_data(graphics()->current_frame());
}

void mesh_renderer::post_render() {
    _group->clear();
}

void mesh_renderer::destroy() {
    destroy_group(material_path, mesh_path, uniforms);
}