#pragma once
#include "Common.h"
// glm uses the right hand system, if you want a left hand system you have to configure glm with a macro
// the reason you cannot use right and left handed system simultaneously is because certain operations 
// like cross products and rotations differ depending on the system, so transitioning between the two would require 
// determining the handedness and separate codes for both systems
// which would introduce unnecessary runtime overhead
// most apis follow the same restriction for this exact reason

// example of a common coordinate preset
struct ZBackward {
    static inline const glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    static inline const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    static inline const glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    static inline const glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
};

// the default coordinate system
struct XForward {
    static inline const glm::vec3 forward = glm::vec3(1.0f, 0.0f, 0.0f);
    static inline const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    static inline const glm::vec3 right = glm::vec3(0.0f, 0.0f, 1.0f);
    static inline const glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
};

// static coordinate systems are defined at compile time with a preset and cannot be moved or rotated
// static coordinate systems use a default glm system as the world space (default glm system is the right hand one)
template<typename SystemPreset = ZBackward>
class StaticCoordinateSystem {
public:

    static_assert(std::is_same<glm::vec3, decltype(SystemPreset::right)>::value, "Right vector must be of type glm::vec3");
    static_assert(std::is_same<glm::vec3, decltype(SystemPreset::up)>::value, "Up vector must be of type glm::vec3");
    static_assert(std::is_same<glm::vec3, decltype(SystemPreset::forward)>::value, "Forward vector must be of type glm::vec3");
    static_assert(std::is_same<glm::vec3, decltype(SystemPreset::origin)>::value, "Origin vector must be of type glm::vec3");

    static_assert(glm::length(SystemPreset::right) == 1.0f, "Right vector must be normalized");
    static_assert(glm::length(SystemPreset::up) == 1.0f, "Up vector must be normalized");
    static_assert(glm::length(SystemPreset::forward) == 1.0f, "Forward vector must be normalized");

    static_assert(glm::dot(SystemPreset::right, SystemPreset::up) == 0.0f, "Right and Up vectors must be orthogonal");
    static_assert(glm::dot(SystemPreset::up, SystemPreset::forward) == 0.0f, "Up and Forward vectors must be orthogonal");
    static_assert(glm::dot(SystemPreset::forward, SystemPreset::right) == 0.0f, "Forward and Right vectors must be orthogonal");

#ifdef GLM_FORCE_RIGHT_HANDED
    static_assert(glm::dot(
        glm::cross(SystemPreset::right, SystemPreset::up),
        SystemPreset::forward
    ) > 0.0, "Must use right hand system");
#elif GLM_FORCE_LEFT_HANDED
    static_assert(glm::dot(
        glm::cross(SystemPreset::right, SystemPreset::up),
        SystemPreset::forward
    ) < 0.0, "Must use left hand system");
#endif

    static inline const glm::vec3& right = SystemPreset::right;
    static inline const glm::vec3& up = SystemPreset::up;
    static inline const glm::vec3& forward = SystemPreset::forward;
    static inline const glm::vec3& origin = SystemPreset::origin;

    static inline const glm::mat4 transformMatrixLocalToWorld = glm::mat4(
        glm::vec4(right, 0.0f),
        glm::vec4(up, 0.0f),
        glm::vec4(forward, 0.0f),
        glm::vec4(origin, 1.0f)
    );

    static inline const glm::mat4 transformMatrixWorldToLocal = []() {
        glm::mat3 inverseRotation = glm::mat3(
            right.x, up.x, forward.x,
            right.y, up.y, forward.y,
            right.z, up.z, forward.z
        );

        // Transform the origin by the inverse rotation
        glm::vec3 inverseTranslation = -inverseRotation * origin;

        // Construct the full inverse transform
        return glm::mat4(
            glm::vec4(inverseRotation[0], 0.0f),
            glm::vec4(inverseRotation[1], 0.0f),
            glm::vec4(inverseRotation[2], 0.0f),
            glm::vec4(inverseTranslation, 1.0f)
        ); 
    }();

#ifdef _DEBUG
    static constexpr bool verifyTransformationReversible(const glm::vec3& testPoint) {
        const float epsilon = 1e-6f;
        glm::vec3 transformed = localToWorld(testPoint);
        glm::vec3 roundTrip = worldToLocal(transformed);
        return glm::length(testPoint - roundTrip) < epsilon;
    }

    static constexpr bool verifyTransformationMatricesReversible(const glm::vec3& testPoint) {
        const float epsilon = 1e-6f;
        glm::vec4 transformed = transformMatrixLocalToWorld * glm::vec4(testPoint, 1.0f);
        glm::vec4 roundTrip = transformMatrixWorldToLocal * transformed;
        return glm::length(testPoint - glm::vec3(roundTrip)) < epsilon;
    }

    static constexpr bool verifyTransformationMethodsEquivalent(const glm::vec3& testPoint) {
        const float epsilon = 1e-6f;
        glm::vec3 transformedDirect = localToWorld(testPoint);
        glm::vec3 transformedMatrix = glm::vec3(transformMatrixLocalToWorld * glm::vec4(testPoint, 1.0f));
        return glm::length(transformedDirect - transformedMatrix) < epsilon;
    }

    static constexpr bool verifyTransformationReversibleComprehensive() {
        const std::vector<glm::vec3> testPoints = {
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 1.0f}
        };

        for (const auto& point : testPoints) {
            if (!verifyTransformationReversible(point) ||
                !verifyTransformationMatricesReversible(point) ||
                !verifyTransformationMethodsEquivalent(point)) {
                return false;
            }
        }
        return true;
    }

    static_assert(verifyTransformationReversibleComprehensive(), "Debug error, transformation is not reversible");
#endif

    // Transform a point from local space to world space
    static glm::vec3 localToWorld(const glm::vec3& localPoint) {
        return origin + localPoint.x * right +
            localPoint.y * up +
            localPoint.z * forward;
    }

    // Transform a point from world space to local space
    static glm::vec3 worldToLocal(const glm::vec3& worldPoint) {
        glm::vec3 relativePoint = worldPoint - origin;
        return glm::vec3(
            glm::dot(relativePoint, right),
            glm::dot(relativePoint, up),
            glm::dot(relativePoint, forward)
        );
    }

    //direction variants don't account for the m_origin
    // Transform a direction from local space to world space
    static glm::vec3 directionLocalToWorld(const glm::vec3& localDirection) {
        return localDirection.x * right +
            localDirection.y * up +
            localDirection.z * forward;
    }

    // Transform a direction from world space to local space
    static glm::vec3 directionWorldToLocal(const glm::vec3& worldDirection) {
        return glm::vec3(
            glm::dot(worldDirection, right),
            glm::dot(worldDirection, up),
            glm::dot(worldDirection, forward)
        );
    }

    // Transform point from another coordinate system to this one
    template<typename OtherSystemPreset>
    static glm::vec3 transformFromOther(const glm::vec3& point) {
        if constexpr (std::is_same_v<SystemPreset, OtherSystemPreset>) {
            return point; // Skip transformation if systems are the same
        }
        return worldToLocal(StaticCoordinateSystem<OtherSystemPreset>::localToWorld(point));
    }

    // Transform point from this coordinate system to another
    template<typename OtherSystemPreset>
    static glm::vec3 transformToOther(const glm::vec3& point) {
        if constexpr (std::is_same_v<SystemPreset, OtherSystemPreset>) {
            return point; // Skip transformation if systems are the same
        }
        return StaticCoordinateSystem<OtherSystemPreset>::worldToLocal(localToWorld(point));
    }

    // Transform direction vector from other system to this one
    // Note: Directions don't use origin offsets
    template<typename OtherSystemPreset>
    static glm::vec3 transformDirectionFromOther(const glm::vec3& direction) {
        if constexpr (std::is_same_v<SystemPreset, OtherSystemPreset>) {
            return direction; // Skip transformation if systems are the same
        }
        return directionWorldToLocal(StaticCoordinateSystem<OtherSystemPreset>::directionLocalToWorld(direction));
    }

    template<typename OtherSystemPreset>
    static glm::vec3 transformDirectionToOther(const glm::vec3& direction) {
        if constexpr (std::is_same_v<SystemPreset, OtherSystemPreset>) {
            return direction; // Skip transformation if systems are the same
        }
        return StaticCoordinateSystem<OtherSystemPreset>::directionWorldToLocal(directionLocalToWorld(direction));
    }
};


class DynamicCoordinateSystem {
private:
    glm::vec3 m_right;
    glm::vec3 m_up;
    glm::vec3 m_forward;
    glm::vec3 m_origin;

public:
    // Constructor taking forward and up vectors
    // Right will be computed to ensure orthogonality
    DynamicCoordinateSystem(const glm::vec3& forward = ZBackward::forward,
        const glm::vec3& up = ZBackward::up, glm::vec3 origin = ZBackward::origin) {
        m_origin = origin;
        setFromForwardUp(forward, up);
    }

    // Alternative constructor taking rotation in euler angles
    DynamicCoordinateSystem(float pitch, float yaw, float roll, glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f)) {
        glm::mat4 rotation = glm::eulerAngleYXZ(yaw, pitch, roll);
        glm::vec3 forward = -glm::vec3(rotation[2]); // -Z column
        glm::vec3 up = glm::vec3(rotation[1]);       // Y column
        m_origin = origin;
        setFromForwardUp(forward, up);
    }

    // Constructor from quaternion
    DynamicCoordinateSystem(const glm::quat& rotation, glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f)) {
        glm::mat4 rotMat = glm::mat4_cast(rotation);
        glm::vec3 forward = -glm::vec3(rotMat[2]);
        glm::vec3 up = glm::vec3(rotMat[1]);
        m_origin = origin;
        setFromForwardUp(forward, up);
    }

    // Set basis vectors ensuring orthogonality
    void setFromForwardUp(glm::vec3 forward, glm::vec3 up) {
        // Normalize input vectors
        forward = glm::normalize(forward);
        up = glm::normalize(up);

        // Compute right vector as cross product of forward and up
        m_right = glm::normalize(glm::cross(up, forward));

        // Recompute up vector to ensure perfect orthogonality
        m_forward = forward;
        m_up = glm::normalize(glm::cross(m_forward, m_right));
    }

    // Get transform matrix
    glm::mat4 getLocalToWorldMatrix() const {
        return glm::mat4(
            glm::vec4(m_right, 0.0f),
            glm::vec4(m_up, 0.0f),
            glm::vec4(m_forward, 0.0f),
            glm::vec4(m_origin, 1.0f)
        );
    }

    glm::mat4 getWorldToLocalMatrix() const {
        // For orthonormal basis, the inverse rotation is the transpose
        glm::mat3 inverseRotation = glm::mat3(
            m_right.x, m_up.x, m_forward.x,
            m_right.y, m_up.y, m_forward.y,
            m_right.z, m_up.z, m_forward.z
        );

        // Transform the origin by the inverse rotation
        glm::vec3 inverseTranslation = -inverseRotation * m_origin;

        // Construct the full inverse transform
        return glm::mat4(
            glm::vec4(inverseRotation[0], 0.0f),
            glm::vec4(inverseRotation[1], 0.0f),
            glm::vec4(inverseRotation[2], 0.0f),
            glm::vec4(inverseTranslation, 1.0f)
        );
    }

    // Move the coordinate system
    void move(const glm::vec3& deltaPos) {
        m_origin += deltaPos;
    }

    void setOrigin(const glm::vec3& origin) { m_origin = origin; }

    // Rotate the coordinate system, all the rotation parameters (axis and quat) are assumed to be relative to world space
    void rotate(const glm::quat& rotation) {
        glm::mat4 rotMat = glm::mat4_cast(rotation);
        m_right = glm::vec3(rotMat * glm::vec4(m_right, 0.0f));
        m_up = glm::vec3(rotMat * glm::vec4(m_up, 0.0f));
        m_forward = glm::vec3(rotMat * glm::vec4(m_forward, 0.0f));
    }

    void rotateAround(const glm::vec3& axis, float angleRadians) {
        glm::quat rotation = glm::angleAxis(angleRadians, glm::normalize(axis));
        rotate(rotation);
    }

    // Overloads for common rotations
    void rotateAroundRight(float angleRadians) {
        rotateAround(m_right, angleRadians);
    }

    void rotateAroundUp(float angleRadians) {
        rotateAround(m_up, angleRadians);
    }

    void rotateAroundForward(float angleRadians) {
        rotateAround(m_forward, angleRadians);
    }

    // rotates the origin and direction vectors around a point
    void rotateAroundPoint(const glm::vec3& point, const glm::vec3& axis, float angleRadians) {
        // Store the original origin
        glm::vec3 originalOrigin = m_origin;

        // Translate to rotation point
        m_origin -= point;

        // Create rotation quaternion
        glm::quat rotation = glm::angleAxis(angleRadians, glm::normalize(axis));

        // Rotate the basis vectors
        rotate(rotation);

        // Rotate the origin point around the rotation point
        m_origin = glm::vec3(glm::mat4_cast(rotation) * glm::vec4(m_origin, 1.0f));

        // Translate back
        m_origin += point;
    }

    // rotates ONLY the origin around a point
    void rotateOriginAroundPoint(const glm::vec3& point, const glm::vec3& axis, float angleRadians) {
        // Store the original origin
        glm::vec3 originalOrigin = m_origin;

        // Translate to rotation point
        m_origin -= point;

        // Create rotation quaternion
        glm::quat rotation = glm::angleAxis(angleRadians, glm::normalize(axis));

        // Rotate the origin point around the rotation point
        m_origin = glm::vec3(glm::mat4_cast(rotation) * glm::vec4(m_origin, 1.0f));

        // Translate back
        m_origin += point;
    }

    // Transform a point from local space to world space
    glm::vec3 localToWorld(const glm::vec3& localPoint) const {
        return m_origin + localPoint.x * m_right +
            localPoint.y * m_up +
            localPoint.z * m_forward;
    }

    // Transform a point from world space to local space
    glm::vec3 worldToLocal(const glm::vec3& worldPoint) const {
        glm::vec3 relativePoint = worldPoint - m_origin;
        return glm::vec3(
            glm::dot(relativePoint, m_right),
            glm::dot(relativePoint, m_up),
            glm::dot(relativePoint, m_forward)
        );
    }

    //direction variants don't account for the m_origin
    // Transform a direction from local space to world space
    glm::vec3 directionLocalToWorld(const glm::vec3& localDirection) const {
        return localDirection.x * m_right +
            localDirection.y * m_up +
            localDirection.z * m_forward;
    }

    // Transform a direction from world space to local space
    glm::vec3 directionWorldToLocal(const glm::vec3& worldDirection) const {
        return glm::vec3(
            glm::dot(worldDirection, m_right),
            glm::dot(worldDirection, m_up),
            glm::dot(worldDirection, m_forward)
        );
    }

    // Transform point from another coordinate system to this one
    glm::vec3 transformFromOther(const DynamicCoordinateSystem& other,
        const glm::vec3& point) const {
        return worldToLocal(other.localToWorld(point));
    }

    // Transform point from this coordinate system to another
    glm::vec3 transformToOther(const DynamicCoordinateSystem& other,
        const glm::vec3& point) const {
        return other.worldToLocal(localToWorld(point));
    }

    // Transform direction vector from other system to this one
    // Note: Directions don't use origin offsets
    glm::vec3 transformDirectionFromOther(const DynamicCoordinateSystem& other,
        const glm::vec3& direction) const {
        // Transform direction to world space then to local space
        return directionWorldToLocal(other.directionLocalToWorld(direction));
    }

    glm::vec3 transformDirectionToOther(const DynamicCoordinateSystem& other,
        const glm::vec3& direction) const {
        // Transform direction to world space then to local space
        return other.directionWorldToLocal(directionLocalToWorld(direction));
    }

    // Transform point from another coordinate system to this one
    template<typename OtherSystemPreset>
    static glm::vec3 transformFromOther(const glm::vec3& point) {
        return worldToLocal(StaticCoordinateSystem<OtherSystemPreset>::localToWorld(point));
    }

    // Transform point from this coordinate system to another
    template<typename OtherSystemPreset>
    static glm::vec3 transformToOther(const glm::vec3& point) {
        return StaticCoordinateSystem<OtherSystemPreset>::worldToLocal(localToWorld(point));
    }

    // Transform direction vector from other system to this one
    // Note: Directions don't use origin offsets
    template<typename OtherSystemPreset>
    static glm::vec3 transformDirectionFromOther(const glm::vec3& direction) {
        return directionWorldToLocal(StaticCoordinateSystem<OtherSystemPreset>::directionLocalToWorld(direction));
    }

    template<typename OtherSystemPreset>
    static glm::vec3 transformDirectionToOther(const glm::vec3& direction) {
        return StaticCoordinateSystem<OtherSystemPreset>::directionWorldToLocal(directionLocalToWorld(direction));
    }

    // Getters
    const glm::vec3& right() const { return m_right; }
    const glm::vec3& up() const { return m_up; }
    const glm::vec3& forward() const { return m_forward; }
    const glm::vec3& getOrigin() const { return m_origin; }
};

