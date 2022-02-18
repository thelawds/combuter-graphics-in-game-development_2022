#include "raytracer_renderer.h"

#include "utils/resource_utils.h"

#include <iostream>


void cg::renderer::ray_tracing_renderer::init()
{
	// Load model
	model = std::make_shared<cg::world::model>();
	model->load_obj(settings->model_path);

	// Create camera
	camera = std::make_shared<cg::world::camera>();
	camera->set_width(static_cast<float>(settings->width));
	camera->set_height(static_cast<float>(settings->height));
	camera->set_position(float3{
			settings->camera_position[0],
			settings->camera_position[1],
			settings->camera_position[2]});
	camera->set_theta(settings->camera_theta);
	camera->set_phi(settings->camera_phi);
	camera->set_angle_of_view(settings->camera_angle_of_view);
	camera->set_z_near(settings->camera_z_near);
	camera->set_z_far(settings->camera_z_far);

	// Create render target
	render_target = std::make_shared<resource<unsigned_color>>(settings->width, settings->height);


	// Create raytracer
	raytracer = std::make_shared<cg::renderer::raytracer<cg::vertex, cg::unsigned_color>>();
	raytracer->set_viewport(settings->width, settings->height);
	raytracer->set_render_target(render_target);
	raytracer->set_vertex_buffers(model->get_vertex_buffers());
	raytracer->set_index_buffers(model->get_index_buffers());

	lights.push_back({
			float3{0, 1.58f, -0.03f},
			float3{0.78f, 0.78f, .78f}
    });

	shadow_raytracer = std::make_shared<cg::renderer::raytracer<cg::vertex, cg::unsigned_color>>();
}

void cg::renderer::ray_tracing_renderer::destroy() {}

void cg::renderer::ray_tracing_renderer::update() {}

void cg::renderer::ray_tracing_renderer::render()
{
	raytracer->clear_render_target({55, 55, 55});
	raytracer->build_acceleration_structure();
	shadow_raytracer->acceleration_structures = raytracer->acceleration_structures;

	raytracer->closest_hit_shader = [&](const ray& ray, payload& payload, const triangle<cg::vertex>& triangle, size_t depth){
		auto position = ray.position + ray.direction * payload.t;
		auto normal = normalize(payload.bary.x * triangle.na + payload.bary.y * triangle.nb + payload.bary.z * triangle.nc);
		float3 result_color {.0f, .0f, .0f};

		for (auto& light: lights) {
			cg::renderer::ray to_light(position, light.position - position);
			auto shadow_payload = shadow_raytracer->trace_ray(to_light, 1, length(light.position - position));
			if (shadow_payload.t < 0.f){
				result_color += triangle.diffuse * light.color * std::max(dot(normal, to_light.direction), .0f);
			} else {

			}
		}

		payload.color = cg::color::from_float3(result_color);

		return payload;
	};

	raytracer->miss_shader = [](const ray& r) {
		payload p;

		p.color = {
				(r.direction.y + 1.f) / 2.f,
				0.f,
				0.f
		};

		return p;
	};

	shadow_raytracer->miss_shader = [](const ray& r) {
		payload p;
		p.t = -1.f;
		return p;
	};

	shadow_raytracer->any_hit_shader = [](const ray& r, payload p, const triangle<cg::vertex>& t){
		return p;
	};

	auto start = std::chrono::high_resolution_clock::now();

	raytracer->ray_generation(
			camera->get_position(), camera->get_direction(), camera->get_right(), camera->get_up(),
			settings->raytracing_depth, settings->accumulation_num);

	auto stop = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = stop - start;
	std::cout << duration.count() << " ms";

	utils::save_resource(*render_target, settings->result_path);
}