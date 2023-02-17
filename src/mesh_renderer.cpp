#include <MARS_3D/mesh_renderer.hpp>
#include <MARS/resources/resource_manager.hpp>
#include <MARS/executioner/executioner.hpp>
#include <MARS/graphics/attribute/vertex3.hpp>
#include <MARS/math/vector4.hpp>

using namespace mars_resources;
using namespace mars_graphics;
using namespace mars_math;
using namespace mars_loader;
using namespace mars_3d;

pl::safe_map<std::pair<std::string, std::string>, mesh_group*> multi_meshes;

mesh_group* get_mesh_group(const std::string& _mat, const std::string& _mesh, graphics_instance* _instance) {
    auto pair = std::make_pair(_mat, _mesh);

    multi_meshes.lock();
    if (multi_meshes.contains(pair)) {
        multi_meshes.unlock();
        return multi_meshes[pair];
    }

    auto group = new mesh_group(_mat, _mesh, _instance);

    multi_meshes[pair] = group;
    multi_meshes.unlock();

    return group;
}

void destroy_group(const std::string& _mat, const std::string& _mesh, shader_data* _uniform) {
    auto pair = std::make_pair(_mat, _mesh);

    multi_meshes.lock();
    if (multi_meshes[pair]->destroy_uniform(_uniform)) {
        multi_meshes[pair]->destroy();
        delete multi_meshes[pair];
        multi_meshes.erase(pair);
    }
    multi_meshes.unlock();
}


mesh_group::mesh_group(const std::string& _mat, const std::string& _mesh, graphics_instance* _instance) {
    m_uniforms.set(nullptr);

    resource_manager::load_resource(_mesh, m_mesh);

    m_instance = _instance;
    m_input = m_instance->instance<shader_input>();

    resource_manager::load_resource(_mat, m_mat, m_instance);
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

void mesh_group::add_uniform(shader_data* _uniforms) {
    auto head = m_uniforms.lock_get();

    if (head == nullptr) {
        m_uniforms.set(_uniforms);
        m_uniforms.unlock();
        return;
    }

    while (head->next() != nullptr)
        head = head->next();

    head->set_next(_uniforms);

    m_uniforms.unlock();
}

void mesh_group::draw() {
    if (m_draw_executed.exchange(true))
        return;

    std::map<size_t, texture*> active_textures;

    m_mat->bind();
    m_input->bind();

    auto head = m_uniforms.lock_get();

    while (head != nullptr) {
        head->bind();
        m_instance->primary_buffer()->draw_indexed(m_mesh->indices.size());

        head = head->next();
    }

    m_uniforms.unlock();
}

void mesh_group::clear() {
    m_draw_executed = false;
}

bool mesh_group::destroy_uniform(mars_graphics::shader_data* _uniform) {
    auto current = m_uniforms.lock_get();
    shader_data* prev = nullptr;

    while (current != _uniform && current->next() != nullptr) {
        prev = current;
        current = current->next();
    }

    if (current == nullptr) {
        m_uniforms.unlock();
        return false;
    }

    if (prev != nullptr)
        prev->set_next(current->next());

    if (m_uniforms.get() == current)
        m_uniforms.set(current->next());

    current->destroy();
    delete current;

    m_uniforms.unlock();

    return m_uniforms.get() == nullptr;
}

void mesh_group::destroy() {
    m_input->destroy();
    delete m_input;
}

void mesh_renderer::load() {
    _group = get_mesh_group(material_path, mesh_path, g_instance());

    render_job = new mars_executioner::executioner_job(_group->get_material()->get_pipeline(), [&]() {
        _group->draw();
    });

    uniforms = _group->get_material()->generate_shader_data();

    _group->add_uniform(uniforms);

    uniforms->update("position", &m_update_mat);
}

void mesh_renderer::send_to_gpu() {
    m_update_mat.transform = g_instance()->get_camera().get_proj_view() * transform().matrix();
    m_update_mat.model = transform().matrix();
    m_update_mat.normal = matrix3<float>(transform().matrix()).inverse().transpose();
    uniforms->get_uniform("position")->copy_data(g_instance()->current_frame());
}

void mesh_renderer::post_render() {
    _group->clear();
}

void mesh_renderer::destroy() {
    destroy_group(material_path, mesh_path, uniforms);
}