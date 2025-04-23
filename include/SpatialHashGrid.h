#include <unordered_map>
#include <vector>
#include <utility>
#include <cmath>
#include<_common.h>
#include "_enemy.h" // Include your enemy class definition
#include <set>

// Hash function for pair<int, int>
struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2; // Simple combination
    }
};

class SpatialHashGrid {
public:
    SpatialHashGrid(float cellSize) : cellSize(cellSize) {}

    // Add an enemy to all cells it overlaps
    void addObject(_enemy* enemy, const AABB& aabb) {
        int minX = static_cast<int>(std::floor(aabb.minX / cellSize));
        int minY = static_cast<int>(std::floor(aabb.minY / cellSize));
        int maxX = static_cast<int>(std::floor(aabb.maxX / cellSize));
        int maxY = static_cast<int>(std::floor(aabb.maxY / cellSize));

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                cells[{x, y}].push_back(enemy);
            }
        }
    }

    // Clear the grid
    void clear() {
        cells.clear();
    }

    // Get potential collision candidates for an AABB
    std::vector<_enemy*> getPotentialCollisions(const AABB& aabb) {
        std::vector<_enemy*> potential;
        int minX = static_cast<int>(std::floor(aabb.minX / cellSize));
        int minY = static_cast<int>(std::floor(aabb.minY / cellSize));
        int maxX = static_cast<int>(std::floor(aabb.maxX / cellSize));
        int maxY = static_cast<int>(std::floor(aabb.maxY / cellSize));

        std::set<_enemy*> uniqueEnemies; // Avoid duplicates
        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                auto it = cells.find({x, y});
                if (it != cells.end()) {
                    for (_enemy* enemy : it->second) {
                        if (uniqueEnemies.insert(enemy).second) {
                            potential.push_back(enemy);
                        }
                    }
                }
            }
        }
        return potential;
    }

private:
    float cellSize;
    std::unordered_map<std::pair<int, int>, std::vector<_enemy*>, PairHash> cells;
};
