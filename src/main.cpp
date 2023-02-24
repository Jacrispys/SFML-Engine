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
    constexpr uint32_t window_x = 1000;
    constexpr uint32_t window_y = 1000;
    const uint32_t frame_rate = 60;

    const float spawn_delay = 0.025f;
    const float spawn_speed = 1200.0f;
    const sf::Vector2f spawn_pos = {(float) window_x / 2, window_y - (window_y - 128)};
    const float object_radius = 10.0f;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;

    auto window = sf::RenderWindow{ sf::VideoMode{ window_x, window_y }, "SFML Engine", sf::Style::Default, settings};
    window.setFramerateLimit(frame_rate);

    Grid* grid = new Grid{{object_radius, object_radius},
              {(int) window_x, (int) window_y}};
    grid->defineGrid();

    Physics physics{grid};
    Render render{window};




    physics.setSimulationUpdateRate(60);
    physics.setSubStepCount(10);
    physics.setConstraint({window_x * 0.5f, window_y * 0.5f}, (float) window_y / 2);

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


        if (clock.getElapsedTime().asSeconds() >= spawn_delay && physics.getObjectsCount() < 1000) {
            clock.restart();
            const float t = physics.getTime();
            const float angle = 1.0f * sin(t) + (3.141592653f) * 0.5f;
            auto& object = physics.addObject(spawn_pos, object_radius);
            physics.setObjectVelocity(object, (spawn_speed * 1) * sf::Vector2f{(float) cos(angle), (float) sin(angle)});
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