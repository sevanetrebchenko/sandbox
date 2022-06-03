
#pragma once

#include <utility>

#include "common/utility/log.h"

namespace Sandbox {

    template <typename T>
    ScalarType<T>::ScalarType() : IDataType() {
        // Supporting only the data types that are valid in shaders.
        static_assert(std::is_same_v<T, bool> ||
                      std::is_same_v<T, int> ||
                      std::is_same_v<T, unsigned> ||
                      std::is_same_v<T, float> ||
                      std::is_same_v<T, double>,
            "Invalid data type: see 'https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)' for valid shader data types.");

        if constexpr (std::is_same_v<T, bool>) {
            dataType = DataType::BOOL;
        }
        else if constexpr (std::is_same_v<T, int>) {
            dataType = DataType::INT;
        }
        else if constexpr (std::is_same_v<T, unsigned>) {
            dataType = DataType::UINT;
        }
        else if constexpr (std::is_same_v<T, float>) {
            dataType = DataType::FLOAT;
        }
        else if constexpr (std::is_same_v<T, double>) {
            dataType = DataType::DOUBLE;
        }
    }

    template <typename T>
    ScalarType<T>::~ScalarType() = default;


    template <typename T>
    VectorType<T>::VectorType(u32 inNumElements) : IDataType(),
                                                   numElements(inNumElements)
                                                   {
        // Validate input.
        // Supporting only the data types that are valid in shaders.
        static_assert(std::is_same_v<T, bool> ||
                      std::is_same_v<T, int> ||
                      std::is_same_v<T, unsigned> ||
                      std::is_same_v<T, float> ||
                      std::is_same_v<T, double>,
            "Invalid data type: see 'https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)' for valid shader data types.");

        std::string type;
        if constexpr (std::is_same_v<T, bool>) {
            type = "bool";
        }
        else if constexpr (std::is_same_v<T, int>) {
            type = "int";
        }
        else if constexpr (std::is_same_v<T, unsigned>) {
            type = "unsigned";
        }
        else if constexpr (std::is_same_v<T, float>) {
            type = "float";
        }
        else { // if constexpr (std::is_same_v<T, double>) {
            type = "double";
        }

        // Ensure VectorType is being used properly.
        if (numElements == 1) {
            ImGuiLog::Instance().LogError("Invalid number of elements in VectorType<%s>::VectorType constructor (received 1, expecting 2, 3, or 4). Did you mean to use ScalarType<%s>?", type.c_str(), type.c_str());
            throw std::runtime_error("VectorType<" + type + ">::VectorType constructor failed - see out/log.txt for more details.");
        }
        else if (numElements < 1 || numElements > 4) {
            ImGuiLog::Instance().LogError("Invalid number of elements in VectorType<%s>::VectorType constructor (received %i, expecting 2, 3, or 4).", type.c_str(), numElements);
            throw std::runtime_error("VectorType<" + type + ">::VectorType constructor failed - see out/log.txt for more details.");
        }

        // It is also possible to cast and use addition based on the number of elements in the vector, but this way is clearer (despite being more verbose).
        if constexpr (std::is_same_v<T, bool>) {
            switch (numElements) {
                case 2:
                    dataType = DataType::BVEC2;
                    break;
                case 3:
                    dataType = DataType::BVEC3;
                    break;
                case 4:
                    dataType = DataType::BVEC4;
                    break;
            }
        }
        else if constexpr (std::is_same_v<T, int>) {
            switch (numElements) {
                case 2:
                    dataType = DataType::IVEC2;
                    break;
                case 3:
                    dataType = DataType::IVEC3;
                    break;
                case 4:
                    dataType = DataType::IVEC4;
                    break;
            }
        }
        else if constexpr (std::is_same_v<T, unsigned>) {
            switch (numElements) {
                case 2:
                    dataType = DataType::UVEC2;
                    break;
                case 3:
                    dataType = DataType::UVEC3;
                    break;
                case 4:
                    dataType = DataType::UVEC4;
                    break;
            }
        }
        else if constexpr (std::is_same_v<T, float>) {
            switch (numElements) {
                case 2:
                    dataType = DataType::VEC2;
                    break;
                case 3:
                    dataType = DataType::VEC3;
                    break;
                case 4:
                    dataType = DataType::VEC4;
                    break;
            }
        }
        else { // if constexpr (std::is_same_v<T, double>) {
            switch (numElements) {
                case 2:
                    dataType = DataType::DVEC2;
                    break;
                case 3:
                    dataType = DataType::DVEC3;
                    break;
                case 4:
                    dataType = DataType::DVEC4;
                    break;
            }
        }
    }

    template <typename T>
    VectorType<T>::~VectorType() = default;


    template <typename T>
    MatrixType<T>::MatrixType(unsigned inNumRows, unsigned inNumColumns) : IDataType(),
                                                                           numRows(inNumRows),
                                                                           numColumns(inNumColumns)
                                                                           {
        // Validate input.
        // Supporting only the data types that are valid in shaders.
        static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>, "Matrices can only contain 32/64-bit (single/double precision) floats.");

        std::string type;
        if constexpr (std::is_same_v<T, float>) {
            type = "float";
        }
        else { // if constexpr (std::is_same_v<T, double>) {
            type = "double";
        }

        // Ensure MatrixType is being used properly.
        bool valid = true;
        std::string message;

        if (numRows == 1) {
            if (numColumns == 1) {
                // Should use ScalarType.
                ImGuiLog::Instance().LogError("Invalid matrix dimensions provided to MatrixType<%s>::MatrixType constructor (received mat1x1, expecting mat2/mat2x2, mat2x3, mat2x4, mat3x2, mat3/mat3x3, mat3x4, mat4x3, mat4/mat4x4. Did you mean to use ScalarType<%s>?", type.c_str(), type.c_str());
            }
            else if (numColumns >= 2 && numColumns <= 4) {
                // Should use VectorType.
                ImGuiLog::Instance().LogError("Invalid matrix dimensions provided to MatrixType<%s>::MatrixType constructor (received mat1x%i, expecting mat2/mat2x2, mat2x3, mat2x4, mat3x2, mat3/mat3x3, mat3x4, mat4x3, mat4/mat4x4. Did you mean to use VectorType<%s>?", type.c_str(), numColumns, type.c_str());
            }
            else {
                // Invalid number of columns.
                ImGuiLog::Instance().LogError("Invalid matrix dimensions provided to MatrixType<%s>::MatrixType constructor (received mat1x%i, expecting mat2/mat2x2, mat2x3, mat2x4, mat3x2, mat3/mat3x3, mat3x4, mat4x3, mat4/mat4x4).", type.c_str(), numColumns);
            }

            throw std::runtime_error("MatrixType<" + type + ">::MatrixType constructor failed - see out/log.txt for more details.");
        }

        if (numColumns == 1) {
            if (numRows == 1) {
                // Should use ScalarType.
                ImGuiLog::Instance().LogError("Invalid matrix dimensions provided to MatrixType<%s>::MatrixType constructor (received mat1x1, expecting mat2/mat2x2, mat2x3, mat2x4, mat3x2, mat3/mat3x3, mat3x4, mat4x3, mat4/mat4x4. Did you mean to use ScalarType<%s>?", type.c_str(), type.c_str());
            }
            else if (numRows >= 2 && numRows <= 4) {
                // Should use VectorType.
                ImGuiLog::Instance().LogError("Invalid matrix dimensions provided to MatrixType<%s>::MatrixType constructor (received mat%ix1, expecting mat2/mat2x2, mat2x3, mat2x4, mat3x2, mat3/mat3x3, mat3x4, mat4x3, mat4/mat4x4. Did you mean to use VectorType<%s>?", type.c_str(), numRows, type.c_str());
            }
            else {
                // Invalid number of columns.
                ImGuiLog::Instance().LogError("Invalid matrix dimensions provided to MatrixType<%s>::MatrixType constructor (received mat%ix1, expecting mat2/mat2x2, mat2x3, mat2x4, mat3x2, mat3/mat3x3, mat3x4, mat4x3, mat4/mat4x4).", type.c_str(), numRows);
            }

            throw std::runtime_error("MatrixType<" + type + ">::MatrixType constructor failed - see out/log.txt for more details.");
        }

        if constexpr (std::is_same_v<T, float>) {
            switch (numRows) {
                case 2:
                    switch (numColumns) {
                        case 2:
                            dataType = DataType::MAT2X2;
                            break;
                        case 3:
                            dataType = DataType::MAT2X3;
                            break;
                        case 4:
                            dataType = DataType::MAT2X4;
                            break;
                    }
                    break;
                case 3:
                    switch (numColumns) {
                        case 2:
                            dataType = DataType::MAT3X2;
                            break;
                        case 3:
                            dataType = DataType::MAT3X3;
                            break;
                        case 4:
                            dataType = DataType::MAT3X4;
                            break;
                    }
                    break;
                case 4:
                    switch (numColumns) {
                        case 2:
                            dataType = DataType::MAT4X2;
                            break;
                        case 3:
                            dataType = DataType::MAT4X3;
                            break;
                        case 4:
                            dataType = DataType::MAT4X4;
                            break;
                    }
                    break;
            }
        }
        else { // if constexpr (std::is_same_v<T, double>) {
            switch (numRows) {
                case 2:
                    switch (numColumns) {
                        case 2:
                            dataType = DataType::DMAT2X2;
                            break;
                        case 3:
                            dataType = DataType::DMAT2X3;
                            break;
                        case 4:
                            dataType = DataType::DMAT2X4;
                            break;
                    }
                    break;
                case 3:
                    switch (numColumns) {
                        case 2:
                            dataType = DataType::DMAT3X2;
                            break;
                        case 3:
                            dataType = DataType::DMAT3X3;
                            break;
                        case 4:
                            dataType = DataType::DMAT3X4;
                            break;
                    }
                    break;
                case 4:
                    switch (numColumns) {
                        case 2:
                            dataType = DataType::DMAT4X2;
                            break;
                        case 3:
                            dataType = DataType::DMAT4X3;
                            break;
                        case 4:
                            dataType = DataType::DMAT4X4;
                            break;
                    }
                    break;
            }
        }
    }

    template <typename T>
    MatrixType<T>::~MatrixType() = default;

}