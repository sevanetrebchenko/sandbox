
#pragma once

namespace Sandbox {

    template <typename T>
    IDataType* ShaderCompiler::ConstructPrimitiveType(unsigned numElements) const {
        IDataType* type = nullptr;

        if (numElements == 1) {
            type = new ScalarType<T>();
        }
        else {
            type = new VectorType<T>(numElements);
        }

        return type;
    }

    template <typename T>
    IDataType* ShaderCompiler::ConstructPrimitiveType(unsigned numElements, unsigned numColumns) const {
        IDataType* type = nullptr;

        if (numColumns == 1) {
            type = ConstructPrimitiveType<T>(numElements);
        }
        else {
            type = new MatrixType<T>(numElements, numColumns);
        }

        return type;
    }

}