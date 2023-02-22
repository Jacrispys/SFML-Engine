#pragma once
#ifndef SFML_ENGINE_RENDER_H
#define SFML_ENGINE_RENDER_H

#include <SFML/Graphics.hpp>
#include "Physics.hpp"

float window_x = 1920.0f;
float window_y = 1080.0f;

class Render {
public:
    explicit Render(sf::RenderTarget &target) : m_target{target} {
    }
    void render(const Physics& physics) const {
        // Constrain render window
        const sf::Vector3f constraint = physics.getConstraint();
        sf::CircleShape bg{constraint.z};
        bg.setOrigin(constraint.z, constraint.z);
        bg.setFillColor(sf::Color::Black);
        bg.setPosition(constraint.x, constraint.y);
        bg.setPointCount(4096);
        m_target.draw(bg);

        // Render Objects
        sf::CircleShape circle{1.0f};
        circle.setPointCount(32);
        circle.setOrigin(1.0f, 1.0f);
        const auto& objects = physics.getObjects();
        for (const auto& obj : objects) {
            circle.setPosition(obj.pos_now);
            circle.setScale(obj.radius, obj.radius);
            circle.setFillColor(obj.color);
            m_target.draw(circle);
        }

    };

private:
    sf::RenderTarget &m_target;
};


#endif //SFML_ENGINE_RENDER_H
