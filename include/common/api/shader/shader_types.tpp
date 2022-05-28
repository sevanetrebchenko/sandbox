
#pragma once

#include <utility>

namespace Sandbox {

    template <typename T>
    ScalarType<T>::ScalarType(std::string inName) : IDataType(std::move(inName), sizeof(T)) {
        // Supporting only the data types that are valid in shaders.
        static_assert(std::is_same_v<T,  b8> ||
                      std::is_same_v<T, i32> ||
                      std::is_same_v<T, u32> ||
                      std::is_same_v<T, f32> ||
                      std::is_same_v<T, f64> ||
                      std::is_same_v<T, StructType>,
            "Invalid data type: see 'https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)' for valid shader data types.");
    }

    template <typename T>
    ScalarType<T>::~ScalarType() = default;


    template <typename T>
    VectorType<T>::VectorType(std::string inName, u32 inNumElements) : IDataType(std::move(inName), sizeof(T) * inNumElements),
                                                                       numElements(inNumElements)
                                                                       {
        // Supporting only the data types that are valid in shaders.
        static_assert(std::is_same_v<T,  b8> ||
                      std::is_same_v<T, i32> ||
                      std::is_same_v<T, u32> ||
                      std::is_same_v<T, f32> ||
                      std::is_same_v<T, f64> ||
                      std::is_same_v<T, StructType>,
            "Invalid data type: see 'https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)' for valid shader data types.");
    }

    template <typename T>
    VectorType<T>::~VectorType() = default;

}