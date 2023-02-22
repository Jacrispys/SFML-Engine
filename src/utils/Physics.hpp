#pragma once
#ifndef SFML_ENGINE_PHYSICS_HPP
#define SFML_ENGINE_PHYSICS_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

struct RigidObject {
    sf::Vector2f pos_now;
    sf::Vector2f pos_old;
    sf::Vector2f acceleration;
    float radius = 10.0f;
    sf::Color color = sf::Color::White;

    RigidObject() = default;

    RigidObject(sf::Vector2f position_, float radius_)
            : pos_now{position_}, pos_old{position_}, acceleration{0.0f, 0.0f}, radius{radius_} {}

    void update(float dt) {
        const sf::Vector2f displacement = pos_now - pos_old;

        pos_old = pos_now;
        pos_now = pos_now + displacement + acceleration * (dt * dt);

        acceleration = {};
    }

    void accelerate(sf::Vector2f a) {
        acceleration += a;
    }

    void setVelocity(sf::Vector2f velo, float dt) {
        pos_old = pos_now - (velo * dt);
    }

    void addVelocity(sf::Vector2f velo, float dt) {
        pos_old -= velo * dt;
    }

    [[nodiscard]] sf::Vector2f getVelocity(float dt) const {
        return (pos_now - pos_old) / dt;
    }

};

class Physics {
public:
    Physics() = default;

    RigidObject &addObject(sf::Vector2f pos, float radius) {
        return objects.emplace_back(pos, radius);
    }

    [[nodiscard]] const std::vector<RigidObject> &getObjects() const {
        return objects;
    }

    void update() {
        time += frame_dt;
        const float step_dt = getStepDt();
        for (uint32_t i{sub_steps}; i--;) {
            applyGravity();
            checkCollisions(step_dt);
            applyConstraint();
            updateObjects(step_dt);
        }
    }

    [[nodiscard]]
    float getStepDt() const {
        return frame_dt / static_cast<float>(sub_steps);
    }

    void setSubStepCount(uint32_t steps) {
        sub_steps = steps;
    }

    void setSimulationUpdateRate(uint32_t rate) {
        frame_dt = 1.0f / static_cast<float>(rate);
    }

    void setObjectVelocity(RigidObject &obj, sf::Vector2f velo) {
        obj.setVelocity(velo, getStepDt());
    }

    void setConstraint(sf::Vector2f position, float radius) {
        constraint_center = position;
        constraint_radius = radius;
    }

    [[nodiscard]]
    sf::Vector3f getConstraint() const {
        return {constraint_center.x, constraint_center.y, constraint_radius};
    }

    [[nodiscard]]
    uint64_t getObjectsCount() const {
        return objects.size();
    }

    [[nodiscard]]
    float getTime() const {
        return time;
    }

private:
    uint32_t sub_steps = 1;
    sf::Vector2f gravity = {0.0f, 2000.0f};
    std::vector<RigidObject> objects;
    sf::Vector2f constraint_center;
    float constraint_radius = 100.0f;
    float time = 0.0f;
    float frame_dt = 0.0f;

    void applyGravity() {
        for (auto &obj: objects) {
            obj.accelerate(gravity);
        }
    }

    void updateObjects(float dt) {
        for (auto &obj: objects) {
            obj.update(dt);
        }
    }

    void applyConstraint() {
        for (auto &obj: objects) {
            const sf::Vector2f v = constraint_center - obj.pos_now;
            const float dist = sqrt(v.x * v.x + v.y * v.y);
            if (dist > (constraint_radius - obj.radius)) {
                const sf::Vector2f n = v / dist;
                obj.pos_now = constraint_center - n * (constraint_radius - obj.radius);
            }
        }
    }

    void checkCollisions(float dt) {
        const float response_coef = 0.75f;
        const uint64_t objects_count = objects.size();
        // Iterate on all objects
        for (uint64_t i{0}; i < objects_count; ++i) {
            RigidObject &object_1 = objects[i];
            // Iterate on object involved in new collision pairs
            for (uint64_t k{i + 1}; k < objects_count; ++k) {
                RigidObject &object_2 = objects[k];
                const sf::Vector2f v = object_1.pos_now - object_2.pos_now;
                const float dist2 = v.x * v.x + v.y * v.y;
                const float min_dist = object_1.radius + object_2.radius;
                // Check overlapping
                if (dist2 < min_dist * min_dist) {
                    const float dist = sqrt(dist2);
                    const sf::Vector2f n = v / dist;
                    const float mass_ratio_1 = object_1.radius / (object_1.radius + object_2.radius);
                    const float mass_ratio_2 = object_2.radius / (object_1.radius + object_2.radius);
                    const float delta = 0.5f * response_coef * (dist - min_dist);
                    // Update positions
                    object_1.pos_now -= n * (mass_ratio_2 * delta);
                    object_2.pos_now += n * (mass_ratio_1 * delta);
                }
            }
        }
    }
};


#endif //SFML_ENGINE_PHYSICS_HPP
