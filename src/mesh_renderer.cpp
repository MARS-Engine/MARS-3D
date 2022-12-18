#include <MVRE_3D/mesh_renderer.hpp>
#include <MVRE/resources/resource_manager.hpp>
#include <MVRE/executioner/executioner.hpp>
#include <MVRE/graphics/attribute/vertex2.hpp>
#include <MVRE/math/vector4.hpp>
#include <MVRE/math/matrix4.hpp>
#include <MVRE/math/quaternion.hpp>
#include <MVRE/time/time_helper.hpp>

using namespace mvre_resources;
using namespace mvre_graphics;
using namespace mvre_math;
using namespace mvre_loader;
using namespace mvre_3d;

void mesh_renderer::load() {
    resource_manager::load_resource("engine/assets/mesh/monkey.obj", mesh);

    input = g_instance()->instance<shader_input>();

    auto job = mvre_executioner::executioner_job([&]() {
        resource_manager::load_resource("engine/assets/materials/mesh.mat", render_material, g_instance());
        render_material->set_pipeline<vertex2>();
        render_material->get_pipeline()->set_viewport({0, 0 }, {1920, 1080 }, {0, 1 });

        input->create();
        input->bind();

        auto vertex = input->add_buffer(mesh->vertices.size() * sizeof(wave_vertex), MVRE_MEMORY_TYPE_VERTEX);
        vertex->copy_data(mesh->vertices.data());

        auto index = input->add_buffer(mesh->indices.size() * sizeof(uint32_t), MVRE_MEMORY_TYPE_INDEX);
        index->copy_data(mesh->indices.data());

        input->load_input(vertex2::get_description());

        input->unbind();
        vertex->unbind();
        index->unbind();

        uniforms = render_material->generate_shader_data();
    });

    mvre_executioner::executioner::add_job(mvre_executioner::EXECUTIONER_JOB_PRIORITY_IN_FLIGHT, &job);
    job.wait();

    render_job = new mvre_executioner::executioner_job(render_material->get_pipeline(), [&]() {
        render_material->bind();
        input->bind();
        uniforms->bind();
        g_instance()->primary_buffer()->draw_indexed(mesh->indices.size());
    });
}

quaternion<float> t;

void mesh_renderer::update() {

    transform()->set_rotation(transform()->rotation() * quaternion<float>::from_axis_angle(vector3<float>(0.0f, 1.0f, 0.0f), time_helper::delta_time()));

    auto trans =  g_instance()->get_camera().get_view_proj() * transform()->matrix();

    if (update_job == nullptr) {
        update_job = new mvre_executioner::executioner_job([&]() {
            uniforms->get_uniform("position")->update(&trans);
        });
    }

    mvre_executioner::executioner::add_job(mvre_executioner::EXECUTIONER_JOB_PRIORITY_IN_FLIGHT, update_job);
    update_job->wait();
}

