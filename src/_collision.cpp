#include "_collision.h"

_collision::_collision()
{

}

_collision::~_collision()
{

}

bool _collision::isOBBCollision(const _Bullet& bullet, const _enemy& enemy)
{
    // Get rotated corners
    auto bulletCorners = bullet.getRotatedCorners();
    auto enemyCorners = enemy.getRotatedCorners();

    // Define axes to test (normals of each edge)
    vector<vec3> axes;
    // Bullet axes (edges: 0->1 and 0->3)
    vec3 bulletAxis1 = {bulletCorners[1].x - bulletCorners[0].x, bulletCorners[1].y - bulletCorners[0].y, 0};
    vec3 bulletAxis2 = {bulletCorners[3].x - bulletCorners[0].x, bulletCorners[3].y - bulletCorners[0].y, 0};
    // Normal of bulletAxis1 (perpendicular)
    axes.push_back({-bulletAxis1.y, bulletAxis1.x, 0});
    // Normal of bulletAxis2
    axes.push_back({-bulletAxis2.y, bulletAxis2.x, 0});

    // Enemy axes (edges: 0->1 and 0->3)
    vec3 enemyAxis1 = {enemyCorners[1].x - enemyCorners[0].x, enemyCorners[1].y - enemyCorners[0].y, 0};
    vec3 enemyAxis2 = {enemyCorners[3].x - enemyCorners[0].x, enemyCorners[3].y - enemyCorners[0].y, 0};
    axes.push_back({-enemyAxis1.y, enemyAxis1.x, 0});
    axes.push_back({-enemyAxis2.y, enemyAxis2.x, 0});

    // Normalize axes
    for (auto& axis : axes)
    {
        float mag = sqrt(axis.x * axis.x + axis.y * axis.y);
        if (mag > 0) {
            axis.x /= mag;
            axis.y /= mag;
        }
    }

    // Test each axis
    for (const auto& axis : axes)
    {
        // Project bullet corners onto axis
        float bulletMin = std::numeric_limits<float>::max();
        float bulletMax = std::numeric_limits<float>::lowest();
        for (const auto& corner : bulletCorners)
        {
            float proj = corner.x * axis.x + corner.y * axis.y;
            bulletMin = std::min(bulletMin, proj);
            bulletMax = std::max(bulletMax, proj);
        }

        // Project enemy corners onto axis
        float enemyMin = std::numeric_limits<float>::max();
        float enemyMax = std::numeric_limits<float>::lowest();
        for (const auto& corner : enemyCorners)
        {
            float proj = corner.x * axis.x + corner.y * axis.y;  // Fixed: 'personally' -> 'corner'
            enemyMin = std::min(enemyMin, proj);
            enemyMax = std::max(enemyMax, proj);
        }

        // Check for separation
        if (bulletMax < enemyMin || bulletMin > enemyMax)
        {
            return false;  // Separating axis found, no collision
        }
    }

    return true;  // No separating axis found, collision detected
}

bool _collision::isOBBCollision(const _player& player, const _enemy& enemy)
{
    // Get rotated corners
    auto playerCorners = player.getRotatedCorners();
    auto enemyCorners = enemy.getRotatedCorners();

    // Define axes to test (normals of each edge)
    vector<vec3> axes;
    // Player axes (edges: 0->1 and 0->3)
    vec3 playerAxis1 = {playerCorners[1].x - playerCorners[0].x, playerCorners[1].y - playerCorners[0].y, 0};
    vec3 playerAxis2 = {playerCorners[3].x - playerCorners[0].x, playerCorners[3].y - playerCorners[0].y, 0};
    // Normal of playerAxis1 (perpendicular)
    axes.push_back({-playerAxis1.y, playerAxis1.x, 0});
    // Normal of playerAxis2
    axes.push_back({-playerAxis2.y, playerAxis2.x, 0});

    // Enemy axes (edges: 0->1 and 0->3)
    vec3 enemyAxis1 = {enemyCorners[1].x - enemyCorners[0].x, enemyCorners[1].y - enemyCorners[0].y, 0};
    vec3 enemyAxis2 = {enemyCorners[3].x - enemyCorners[0].x, enemyCorners[3].y - enemyCorners[0].y, 0};
    axes.push_back({-enemyAxis1.y, enemyAxis1.x, 0});
    axes.push_back({-enemyAxis2.y, enemyAxis2.x, 0});

    // Normalize axes
    for (auto& axis : axes)
    {
        float mag = sqrt(axis.x * axis.x + axis.y * axis.y);
        if (mag > 0) {
            axis.x /= mag;
            axis.y /= mag;
        }
    }

    // Test each axis
    for (const auto& axis : axes)
    {
        // Project player corners onto axis
        float playerMin = std::numeric_limits<float>::max();
        float playerMax = std::numeric_limits<float>::lowest();
        for (const auto& corner : playerCorners)
        {
            float proj = corner.x * axis.x + corner.y * axis.y;
            playerMin = std::min(playerMin, proj);
            playerMax = std::max(playerMax, proj);
        }

        // Project enemy corners onto axis
        float enemyMin = std::numeric_limits<float>::max();
        float enemyMax = std::numeric_limits<float>::lowest();
        for (const auto& corner : enemyCorners)
        {
            float proj = corner.x * axis.x + corner.y * axis.y;
            enemyMin = std::min(enemyMin, proj);
            enemyMax = std::max(enemyMax, proj);
        }

        // Check for separation
        if (playerMax < enemyMin || playerMin > enemyMax)
        {
            return false;  // Separating axis found, no collision
        }
    }

    return true;  // No separating axis found, collision detected
}

bool _collision::isOBBCollision(const _player& player, const _xpOrb& orb)
{
    // Simple AABB test using their bounding boxes (since orbs don�t rotate)
    vec3 pMin = player.getCollisionBoxMin();
    vec3 pMax = player.getCollisionBoxMax();
    vec3 oMin = orb.getCollisionBoxMin();
    vec3 oMax = orb.getCollisionBoxMax();

    bool xOverlap = pMin.x <= oMax.x && pMax.x >= oMin.x;
    bool yOverlap = pMin.y <= oMax.y && pMax.y >= oMin.y;
    bool zOverlap = pMin.z <= oMax.z && pMax.z >= oMin.z;

    return xOverlap && yOverlap && zOverlap;
}
