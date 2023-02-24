#pragma once
#ifndef SFML_ENGINE_PHYSICS_HPP
#define SFML_ENGINE_PHYSICS_HPP

#include <SFML/Graphics.hpp>
#include <utility>
#include <vector>
#include <cmath>
#include <iostream>


struct RigidObject {
    sf::Vector2f pos_now;
    sf::Vector2f pos_old;
    sf::Vector2f acceleration;
    float radius = 10.0f;
    sf::Color color = sf::Color::White;
    long object_id;

    RigidObject() = default;

    RigidObject(sf::Vector2f position_, float radius_, long _object_id)
            : pos_now{position_}, pos_old{position_}, acceleration{0.0f, 0.0f}, radius{radius_},
              object_id{_object_id} {}

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

struct Cell {
    Cell(float cell_x, float cell_y, uint32_t id) : cell_size_x(cell_x), cell_size_y(cell_y), cell_id(id) {}

    sf::Color color1 = sf::Color(237, 38, 154, 100);
    sf::Color color2 = sf::Color(0, 0, 0, 100);
    float cell_size_x;
    float cell_size_y;
    sf::Vector2f cell_pos;
    uint32_t cell_id;
    sf::Vector2i grid_map_pos;
    std::vector<RigidObject> objects;

    void updateCell(const std::vector<RigidObject> &object) {
        objects.clear();
        for (auto &obj: object) {
            sf::Vector2f cellX = {((float) grid_map_pos.x * cell_size_x),
                                  ((float) grid_map_pos.x * cell_size_x + cell_size_x)};
            sf::Vector2f cellY = {((float) grid_map_pos.y * cell_size_y),
                                  ((float) grid_map_pos.y * cell_size_y + cell_size_y)};
            if ((obj.pos_now.x >= cellX.x && obj.pos_now.x <= cellX.y) &&
                (obj.pos_now.y >= cellY.x && obj.pos_now.y <= cellY.y)) {
                objects.emplace_back(obj);
            }
        }
    }
};

struct Grid {
    sf::Color color = sf::Color(168, 170, 173, 255);
    std::vector<Cell> cells;
    sf::Vector2f cell_size;
    sf::Vector2i window_size;
    int width = window_size.x / cell_size.x;
    int height = window_size.y / cell_size.y;

    Grid() = default;

    Grid(sf::Vector2f _cell_size, sf::Vector2i _window_size)
            : cell_size{_cell_size}, window_size{_window_size} {}

    void defineGrid() {
        int rows = window_size.x / cell_size.x;
        int cols = window_size.y / cell_size.y;
        std::cout << "rows: " << rows << " cols: " << cols << std::endl;
        std::cout << "Size x: " << cell_size.x << " Size Y: " << cell_size.y << std::endl;

        for (int c = 0; c < cols + 1; c++) {
            for (int r = 0; r < rows + 1; r++) {
                Cell cell(cell_size.x, cell_size.y, (r + c));
                cell.grid_map_pos = {r, c};
                cell.cell_pos = {(r * cell_size.y), (c * cell_size.x)};
                addCell(cell);
            }
        }
        std::cout << "Num Cells: " << cells.size() << std::endl;
    }

    void cellUpdate(std::vector<RigidObject> objects) {
        for (Cell cell : cells) {
            cell.updateCell(objects);
        }
    }

    [[nodiscard]] Cell getCellByMap(int x, int y) const {
        for (Cell cell: getCells()) {
            if (cell.grid_map_pos.x == x && cell.grid_map_pos.y == y) return cell;
        }
        return getCells()[0];
    }

    [[nodiscard]] std::vector<Cell> getCells() const {
        return cells;
    }

    void clearCells() {
        cells.clear();
    }

    void addCell(const Cell &cell) {
        cells.emplace_back(cell);
    }

};

class Physics {
public:
    explicit Physics(Grid *_grid) : grid{_grid} {}

    RigidObject &addObject(sf::Vector2f pos, float radius) {
        return objects.emplace_back(pos, radius, objects.size() + 1);
    }

    [[nodiscard]] std::vector<RigidObject> getObjects() const {
        return objects;
    }

    void update() {
        time += frame_dt;
        const float step_dt = getStepDt();

        for (uint32_t i{sub_steps}; i--;) {
            //grid->cellUpdate(objects);
            applyGravity();
            checkCollisions(step_dt);
            //check_collisions_grid();
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

    void setObjectVelocity(RigidObject &obj, sf::Vector2f velo) const {
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

    void clearObjects() {
        objects.clear();
    }


private:
    uint32_t sub_steps = 1;
    sf::Vector2f gravity = {0.0f, 2000.0f};
    std::vector<RigidObject> objects;
    sf::Vector2f constraint_center;
    float constraint_radius = 100.0f;
    float time = 0.0f;
    float frame_dt = 0.0f;
    Grid *grid;


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


    void check_collisions_grid() {
        for (int x{1}; x < grid->width - 1; ++x) {
            for (int y{1}; y < grid->height - 1; ++y) {
                Cell current_cell = grid->getCellByMap(x, y);

                for (int dx{-1}; dx <= 1; ++dx) {
                    for (int dy{-1}; dy <= 1; ++dy) {
                        Cell other_cell = grid->getCellByMap(x + dx, y + dy);
                        check_cells_collision(current_cell, other_cell);
                    }
                }
            }
        }
    }


    void check_cells_collision(Cell &cell1, Cell &cell2) {
        for (auto &obj_1: cell1.objects) {
            for (auto &obj_2: cell2.objects) {
                if (obj_1.object_id != obj_2.object_id) {
                    if (collide(obj_1, obj_2)) {
                        solve_collisions(obj_1, obj_2);
                    }
                }
            }
        }
    }

    static bool collide(RigidObject object_1, RigidObject object_2) {
        const sf::Vector2f v = object_1.pos_now - object_2.pos_now;
        const float dist2 = v.x * v.x + v.y * v.y;
        const float min_dist = object_1.radius + object_2.radius;
        // Check overlapping
        return (dist2 < min_dist * min_dist);
    }

    static void solve_collisions(RigidObject object_1, RigidObject object_2) {
        const float response_coef = 0.75f;
        const sf::Vector2f v = object_1.pos_now - object_2.pos_now;
        const float dist2 = v.x * v.x + v.y * v.y;
        const float dist = sqrt(dist2);
        const sf::Vector2f n = v / dist;
        const float mass_ratio_1 = object_1.radius / (object_1.radius + object_2.radius);
        const float mass_ratio_2 = object_2.radius / (object_1.radius + object_2.radius);
        const float min_dist = object_1.radius + object_2.radius;
        const float delta = 0.5f * response_coef * (dist - min_dist);
        // Update positions
        object_1.pos_now -= n * (mass_ratio_2 * delta);
        object_2.pos_now += n * (mass_ratio_1 * delta);
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
