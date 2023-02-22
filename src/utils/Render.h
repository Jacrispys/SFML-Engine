#pragma once
#ifndef SFML_ENGINE_RENDER_H
#define SFML_ENGINE_RENDER_H

#include <SFML/Graphics.hpp>


class Render {
public:
    explicit Render(sf::RenderTarget &target) : m_target{target} {
    }
    void render() const {
        // Constrain render window
        const sf::Vector3f constraint = {(1920.0f * 0.5f), (1080.0f * 0.5f), (1028.0f * 0.5f)};
        sf::CircleShape bg{constraint.z};
        bg.setOrigin(constraint.z, constraint.z);
        bg.setFillColor(sf::Color::Black);
        bg.setPosition(constraint.x, constraint.y);
        bg.setPointCount(1028);
        m_target.draw(bg);

    };

private:
    sf::RenderTarget &m_target;
};


#endif //SFML_ENGINE_RENDER_H
