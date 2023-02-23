#include <SFML/Graphics.hpp>
#include "utils/Render.hpp"

static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * (3.141592653));
    const float b = sin(t + 0.66f * 2.0f * (3.141592653));
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}

int main()
{
    auto window = sf::RenderWindow{ { (unsigned int) window_x, (unsigned int) window_y }, "SFML Engine" };
    window.setFramerateLimit(60);

    Physics physics;
    Render render{window};


    physics.setSimulationUpdateRate(60);
    physics.setSubStepCount(10);
    physics.setConstraint({window_x * 0.5f, window_y * 0.5f}, window_y / 2);

    sf::Clock clock;
    sf::Clock frameDelay;
    sf::Font font;
    sf::Text text;

    if (!font.loadFromFile("../src/resources/dealerplate.otf")) {
        std::cout << "Error Loading Font!" << std::endl;
    } else {
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(5.0f, 0.0f);
    }

    while (window.isOpen())
    {
        double delay = frameDelay.getElapsedTime().asMilliseconds() / 16.67;
        int objectCount = physics.getObjectsCount();
        int fps = 1000.0f / (delay * 16.67);
        std::string display = "Object Count: " + std::to_string(objectCount) + "\nFrame Delay: " + std::to_string(delay) + "ms\nFPS: " +
                std::to_string(fps);
        text.setString(display);
        frameDelay.restart();

        if (clock.getElapsedTime().asSeconds() >= 0.0025f) {
            clock.restart();
            const float t = physics.getTime();
            const float angle = 1.0f * sin(t) + (3.141592653f) * 0.5f;
            auto& object = physics.addObject({window_x / 2, window_y - (window_y - 128)}, 3.0f);
            physics.setObjectVelocity(object, (1200.0f * 1) * sf::Vector2f{(float) cos(angle), (float) sin(angle)});
            object.color = getRainbow(t);
        }
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
                std::cout << physics.getObjects().size() << std::endl;
                physics.clearObjects();
            }
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                window.close();
            }
        }

        physics.update();
        window.clear(sf::Color(51, 54, 59, 100));
        render.render(physics);
        window.draw(text);
        window.display();
    }

    return 0;
}