#include <SFML/Graphics.hpp>
#include "utils/Render.hpp"

int main()
{
    auto window = sf::RenderWindow{ { (unsigned int) window_x, (unsigned int) window_y }, "SFML Engine" };
    window.setFramerateLimit(60);

    Physics physics;
    Render render{window};


    physics.setSimulationUpdateRate(60);
    physics.setSubStepCount(8);
    physics.setConstraint({window_x * 0.5f, window_y * 0.5f}, window_y / 2);

    sf::Clock clock;

    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && clock.getElapsedTime().asSeconds() >= 0.025f) {
                clock.restart();
                const float t = physics.getTime();
                const float angle = 1.0f * sin(t) + (3.141592653f) * 0.5f;
                auto& object = physics.addObject({window_x / 2, window_y - (window_y - 128)}, 10.0f);
                physics.setObjectVelocity(object, (1200.0f * 1) * sf::Vector2f{(float) cos(angle), (float) sin(angle)});

            }
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                window.close();
            }
        }

        physics.update();
        window.clear(sf::Color(51, 54, 59, 100));
        render.render(physics);
        window.display();
    }

    return 0;
}