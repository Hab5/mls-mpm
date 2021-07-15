#define GLM_PRECITION_LOWP_FLOAT
#define GLM_FORCE_PURE

#include "Display.hpp"
#include "ResourceManager.hpp"
#include "Callbacks.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/scalar_multiplication.hpp>
#include <glm/gtx/io.hpp>

#include <iostream>
#include <algorithm>
#include <vector>
#include <unistd.h>
#include <random>

const int w = 1024*1.3;
const int h = 768*1.3;

float lastFrame = 0.0f;
float deltaTime = 0.0f;

void Init();

void processInput(GLFWwindow* Window, Camera& camera) {
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(Window, true);
    
    camera.ProcessMouse(deltaTime);

    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Init();
}

void Settings() {
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
}

void Callbacks(GLFWwindow* Window) {
    glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);
    glDebugMessageCallback(debug_msg_callback, 0);
}

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    glm::mat3 C;
    float padding;
};

struct Cell {
    glm::vec3 vel;
    float mass;
    uint32_t index;
    float padding;
};

std::vector<Particle> particles;
std::vector<Cell> grid;
glm::vec3 weights[3];

const float dt = 0.30f;
const int iterations = (int)(1.0f / dt);

const float gravity = -0.3f;
const float particle_mass = 1.0f;

const float rest_density = 6.0f;
const float dynamic_viscosity = 0.1f;

const float eos_stiffness = 10.0f;
const float eos_power = 4;

const float damping = 0.999f;

const int particle_res = 16;
const int grid_res = 45;

void Init() {
    particles.clear();
    std::vector<glm::vec3> tmp_pos;
    const int box_x = 25, box_y = 16, box_z = 16;
    const float sx = grid_res / 2.0f, sy = grid_res / 2.0f, sz = grid_res / 2.0f;
    const float spacing = 0.5f;
    for (float i = sx - box_x / 2; i < sx + box_x / 2; i += spacing) {
        for (float j = sy - box_y / 2; j < sy + box_y / 2; j += spacing) {
            for (float k = sz - box_z / 2; k < sz + box_z / 2; k += spacing) {
                glm::vec3 pos = glm::vec3(i, j, k);
                tmp_pos.push_back(pos);
            }
        }
    }


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rnd_rnd(-5.0, 5.0);
    std::uniform_real_distribution<float> rnd_x(rnd_rnd(gen), rnd_rnd(gen));
    std::uniform_real_distribution<float> rnd_y(rnd_rnd(gen), rnd_rnd(gen));
    std::uniform_real_distribution<float> rnd_z(rnd_rnd(gen), rnd_rnd(gen));

    for (unsigned int i = 0; i < tmp_pos.size(); ++i) {
        particles.push_back({
            .pos = tmp_pos[i],
            .vel = glm::vec3(rnd_x(gen), 0.0f, rnd_z(gen)),
            .C = glm::mat3(0.0f),
        });
    }

    std::cout << particles.size() << std::endl;

    grid.resize(grid_res*grid_res*grid_res);
    for (uint32_t i = 0; i < grid_res*grid_res*grid_res; ++i)
        grid[i].index = i;
}

void Simulate() {
    
    // CLEAR GRID
    // #pragma omp parallel for
    for (auto& cell: grid) {
        cell.vel = glm::vec3(0.0f);
        cell.mass = 0.0f;
    }
    
    // P2G_1        
    // #pragma omp parallel for
    for (int i = 0; i < particles.size(); i++) {
        auto& p = particles[i];
        glm::uvec3 cell_idx = glm::uvec3(p.pos);
        glm::vec3 cell_diff = (p.pos - glm::vec3(cell_idx)) - 0.5f;

        glm::vec3 weights_[3];
        weights_[0] = 0.5f  * glm::pow(0.5f - cell_diff, glm::vec3(2.0f));
        weights_[1] = 0.75f - glm::pow(cell_diff, glm::vec3(2.0f));
        weights_[2] = 0.5f  * glm::pow(0.5f + cell_diff, glm::vec3(2.0f));
        
        for (uint32_t gx = 0; gx < 3; ++gx) {
            for (uint32_t gy = 0; gy < 3; ++gy) {
                for (uint32_t gz = 0; gz < 3; ++gz) {
                    float weight = weights_[gx].x * weights_[gy].y * weights_[gz].z;

                    glm::uvec3 cell_pos = glm::uvec3(cell_idx.x + gx - 1, 
                                                     cell_idx.y + gy - 1,
                                                     cell_idx.z + gz - 1);
                    glm::vec3 cell_dist = (glm::vec3(cell_pos) - p.pos) + 0.5f;
                    glm::vec3 Q = p.C * cell_dist;
                    
                    int cell_index = (int)cell_pos.x + 
                                     (int)cell_pos.y * grid_res +
                                     (int)cell_pos.z * grid_res * grid_res;
            
                    float mass_contrib = weight * particle_mass;
                    grid[cell_index].mass += mass_contrib;
                    grid[cell_index].vel += mass_contrib * (p.vel + Q);
                }

            }
        }
    }
    
    // P2G_2
    // #pragma omp parallel for
    for (int i = 0; i < particles.size(); i++) {
        auto& p = particles[i];

        glm::uvec3 cell_idx = glm::uvec3(p.pos);
        glm::vec3 cell_diff = (p.pos - glm::vec3(cell_idx)) - 0.5f;

        glm::vec3 weights_[3];
        weights_[0] = 0.5f  * glm::pow(0.5f - cell_diff, glm::vec3(2.0f));
        weights_[1] = 0.75f - glm::pow(cell_diff, glm::vec3(2.0f));
        weights_[2] = 0.5f  * glm::pow(0.5f + cell_diff, glm::vec3(2.0f));
        
        float density = 0.0f;
        for (uint32_t gx = 0; gx < 3; ++gx) {
            for (uint32_t gy = 0; gy < 3; ++gy) {
                for (uint32_t gz = 0; gz < 3; ++gz) {
                    float weight = weights_[gx].x * weights_[gy].y * weights_[gz].z;

                    glm::uvec3 cell_pos = glm::uvec3(cell_idx.x + gx - 1, 
                                                     cell_idx.y + gy - 1,
                                                     cell_idx.z + gz - 1);
                    
                    int cell_index = (int)cell_pos.x + 
                                     (int)cell_pos.y * grid_res +
                                     (int)cell_pos.z * grid_res * grid_res;

                    density += grid[cell_index].mass * weight;
                }
            }
        }

        float volume = particle_mass / density;
        float pressure = std::max(-0.1f, eos_stiffness * 
                                (std::pow(density/rest_density, eos_power) - 1.0f));

        glm::mat3 stress = glm::mat3(
            -pressure, 0, 0,
            0, -pressure, 0,
            0, 0, -pressure
        );

        // glm::mat3 strain = p.C;

        // float trace = strain[0][0] + strain[1][0] + strain[2][0]; // DEBUG
        // float trace = glm::determinant(strain);
        // strain[0][0] = strain[1][0] = strain[2][0] = trace;

        // glm::mat3 viscosity_term = dynamic_viscosity * strain;
        // stress += viscosity_term;

        auto eq_16_term_0 = -volume * 4 * stress * dt;
        
        for (uint32_t gx = 0; gx < 3; ++gx) {
            for (uint32_t gy = 0; gy < 3; ++gy) {
                for (uint32_t gz = 0; gz < 3; ++gz) {
                    float weight = weights_[gx].x * weights_[gy].y * weights_[gz].z;

                    glm::uvec3 cell_pos = glm::uvec3(cell_idx.x + gx - 1, 
                                                     cell_idx.y + gy - 1,
                                                     cell_idx.z + gz - 1);

                    glm::vec3 cell_dist = (glm::vec3(cell_pos) - p.pos) + 0.5f;

                    int cell_index = (int)cell_pos.x + 
                                     (int)cell_pos.y * grid_res +
                                     (int)cell_pos.z * grid_res * grid_res;

                    glm::vec3 momentum = (eq_16_term_0 * weight) * cell_dist;
                    grid[cell_index].vel += momentum;
                }
            }
        }
    }

    // GRID UPDATE
    // #pragma omp parallel for
    for (auto& cell: grid) {
        if (cell.mass > 0) {
            cell.vel /= cell.mass;
            cell.vel += dt * glm::vec3(0.0f, gravity, 0.0f);

            int index = cell.index;
            int x = index / (grid_res * grid_res);
            index /= grid_res;
            int y = (index / grid_res) % grid_res;
            index /= grid_res;
            int z = index;
            
            if (x < 1 || x > grid_res - 2) {cell.vel.x = 0.0f;}
            if (y < 1 || y > grid_res - 2) {cell.vel.y = 0.0f;}
            if (z < 1 || z > grid_res - 2) {cell.vel.z = 0.0f;}
        }
    }

    // G2P
    // #pragma omp parallel for
    for (auto& p: particles) {
        
        p.vel = glm::vec3(0.0f);
        
        glm::uvec3 cell_idx = glm::uvec3(p.pos);
        glm::vec3 cell_diff = (p.pos - glm::vec3(cell_idx)) - 0.5f;

        glm::vec3 weights_[3];
        
        weights_[0] = 0.5f  * glm::pow(0.5f - cell_diff, glm::vec3(2.0f));
        weights_[1] = 0.75f - glm::pow(cell_diff, glm::vec3(2.0f));
        weights_[2] = 0.5f  * glm::pow(0.5f + cell_diff, glm::vec3(2.0f));

        glm::mat3 B = glm::mat3(0.0f);
        for (uint32_t gx = 0; gx < 3; ++gx) {
            for (uint32_t gy = 0; gy < 3; ++gy) {
                for (uint32_t gz = 0; gz < 3; ++gz) {
                    float weight = weights_[gx].x * weights_[gy].y * weights_[gz].z;
                    // std::cout << weight << std::endl;
                    glm::uvec3 cell_pos = glm::uvec3(cell_idx.x + gx - 1, 
                                                     cell_idx.y + gy - 1,
                                                     cell_idx.z + gz - 1);

                    glm::vec3 cell_dist = (glm::vec3(cell_pos) - p.pos) + 0.5f;
                    
                    int cell_index = (int)cell_pos.x + 
                                     (int)cell_pos.y * grid_res +
                                     (int)cell_pos.z * grid_res * grid_res;

                    glm::vec3 weighted_velocity = grid[cell_index].vel * weight;

                    B += glm::mat3(weighted_velocity * cell_dist.x, 
                                   weighted_velocity * cell_dist.y,
                                   weighted_velocity * cell_dist.z);

                    p.vel += weighted_velocity;
                }
            }
        }

        p.C = B * 4.0f;
        p.vel *= damping; 
        p.pos += p.vel * dt;
        p.pos = glm::clamp(p.pos, 1.0f, grid_res - 2.0f);

        glm::vec3 x_n = p.pos + p.vel;
        const float wall_min = 3.0f;
        const float wall_max = grid_res - 4.0f;
        if (x_n.x < wall_min) p.vel.x += (wall_min - x_n.x);
        if (x_n.x > wall_max) p.vel.x += (wall_max - x_n.x);
        if (x_n.y < wall_min) p.vel.y += (wall_min - x_n.y);
        if (x_n.y > wall_max) p.vel.y += (wall_max - x_n.y);
        if (x_n.z < wall_min) p.vel.z += (wall_min - x_n.z);
        if (x_n.z > wall_max) p.vel.z += (wall_max - x_n.z); 
    }
}

int main() {

    Display display(w, h, "MLS-MPM");
    Callbacks(display.Window);
    Settings();

    ResourceManager::LoadShader("base", "shaders/base.vert", 
                                        "shaders/base.frag",
                                        "shaders/base.geom");
    
    Camera camera(display.Window, glm::vec3(24.0, 24.0, -60.0));
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), &particles[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, vel));
    glEnableVertexAttribArray(1);
    
    Init();
    while(!glfwWindowShouldClose(display.Window)) {
        deltaTime = glfwGetTime() - lastFrame;
        lastFrame = glfwGetTime();
        printf("%f\n", deltaTime);

        processInput(display.Window, camera);

        // Compute
        Simulate();
        
        // Update buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), &particles[0], GL_DYNAMIC_DRAW);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), &particles[0]);

        // Render
        ResourceManager::GetShader("base").Use();
        ResourceManager::GetShader("base").SetMatrix4("view", camera.GetView());
        ResourceManager::GetShader("base").SetMatrix4("projection", camera.GetProjection());
        ResourceManager::GetShader("base").SetFloat("particle_size", 0.7f);
        
        
        glBindVertexArray(VAO);
        display.Clear(0.05,0.05,0.07,1);
        glDrawArrays(GL_POINTS, 0, particles.size());
        display.SwapBuffers();
        glfwPollEvents();
    }

    ResourceManager::CleanUp();
    return 0;
}
