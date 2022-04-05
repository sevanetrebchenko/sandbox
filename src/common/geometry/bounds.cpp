
#include "common/geometry/bounds.h"

namespace Sandbox {

    Bounds::Bounds() : initialized_(false),
                       minimum_(glm::vec3(0.0f)),
                       maximum_(glm::vec3(0.0f))
                       {
    }

    Bounds::Bounds(const glm::vec3& point) : initialized_(true),
                                             minimum_(point),
                                             maximum_(point)
                                             {
    }

    Bounds::Bounds(const glm::vec3& minimum, const glm::vec3& maximum) : initialized_(true),
                                                                         minimum_(minimum),
                                                                         maximum_(maximum)
                                                                         {
    }

    Bounds::~Bounds() = default;

    void Bounds::Extend(const glm::vec3& point) {
        if (initialized_) {
            minimum_ = glm::min(minimum_, point);
            maximum_ = glm::max(maximum_, point);
        }
        else {
            minimum_ = point;
            maximum_ = point;
            initialized_ = true;
        }
    }

    void Bounds::Extend(float scale) {
        if (initialized_) {
            minimum_ *= scale;
            maximum_ *= scale;
        }
    }

    glm::vec3 Bounds::GetDiagonal() const {
        // Uninitialized returns glm::vec3(0.0f) regardless.
        return maximum_ - minimum_;
    }

    float Bounds::GetSurfaceArea() const {
        // Uninitialized returns 0.0f regardless.

        glm::vec3 diagonal = GetDiagonal();
        return 2.0f * (diagonal.x * diagonal.y + diagonal.x * diagonal.z + diagonal.y * diagonal.z);
    }

    float Bounds::GetVolume() const {
        // Uninitialized returns 0.0f regardless.

        glm::vec3 diagonal = GetDiagonal();
        return diagonal.x * diagonal.y * diagonal.z;
    }

    int Bounds::GetMaximumAxis() const {
        if (!initialized_) {
            return -1;
        }

        glm::vec3 diagonal = GetDiagonal();
        if (diagonal.x > diagonal.y && diagonal.x > diagonal.z) {
            return 0;
        }
        else if (diagonal.y > diagonal.z) {
            return 1;
        }
        else {
            return 2;
        }
    }

    bool Bounds::Inside(const glm::vec3& point) const {
        if (!initialized_) {
            return false;
        }

        // Component-wise 1D containment checks.
        bool inside = true;

        for (int i = 0; i < 3; ++i) {
            // Account for floating point inaccuracy.
            // point[i] >= minimum_[i]
            inside &= ((glm::abs(point[i] - minimum_[i]) < std::numeric_limits<float>::epsilon()) || (point[i] > minimum_[i]));

            // point[i] <= maximum[i]
            inside &= ((point[i] < maximum_[i]) || (glm::abs(point[i] - maximum_[i]) < std::numeric_limits<float>::epsilon()));
        }

        return inside;
    }

    Bounds Bounds::GetUnion(const Bounds& first, const Bounds& second) {
        if (!first.initialized_) {
            if (!second.initialized_) {
                return { };
            }

            return second;
        }
        else {
            if (!second.initialized_) {
                return first;
            }

            // Component-wise maximum.
            return { glm::vec3(glm::min(first.minimum_.x, second.minimum_.x),
                               glm::min(first.minimum_.y, second.minimum_.y),
                               glm::min(first.minimum_.z, second.minimum_.z)),
                     glm::vec3(glm::max(first.maximum_.x, second.maximum_.x),
                               glm::max(first.maximum_.y, second.maximum_.y),
                               glm::max(first.maximum_.z, second.maximum_.z)) };
        }
    }

    Bounds Bounds::GetIntersection(const Bounds& first, const Bounds& second) {
        // Component-wise inverted maxima.
        return { glm::vec3(glm::max(first.minimum_.x, second.minimum_.x),
                           glm::max(first.minimum_.y, second.minimum_.y),
                           glm::max(first.minimum_.z, second.minimum_.z)),
                 glm::vec3(glm::min(first.maximum_.x, second.maximum_.x),
                           glm::min(first.maximum_.y, second.maximum_.y),
                           glm::min(first.maximum_.z, second.maximum_.z)) };
    }

    bool Bounds::Overlap(const Bounds& first, const Bounds& second) {
        // Component-wise 1D containment checks.
        bool overlap = true;

        for (int i = 0; i < 3; ++i) {
            // Account for floating point inaccuracy.
            // first.maximum_[i] >= second.minimum_[i]
            overlap &= ((glm::abs(first.maximum_[i] - second.minimum_[i]) < std::numeric_limits<float>::epsilon()) || (first.maximum_[i] > second.minimum_[i]));

            // first.minimum_[i] <= maximum[i]
            overlap &= ((first.minimum_[i] < second.maximum_[i]) || (glm::abs(first.minimum_[i] - second.maximum_[i]) < std::numeric_limits<float>::epsilon()));
        }

        return overlap;
    }

    const glm::vec3& Bounds::GetMinimum() const {
        return minimum_;
    }

    const glm::vec3& Bounds::GetMaximum() const {
        return maximum_;
    }

    glm::vec3 Bounds::GetCentroid() const {
        return (minimum_ + maximum_) / 2.0f;
    }

}
