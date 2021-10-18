
#ifndef SANDBOX_DEBUG_H
#define SANDBOX_DEBUG_H

#pragma once

#include <sandbox_pch.h>

class DDRenderInterfaceCoreGL final : public dd::RenderInterface {
    public:
        void drawPointList(const dd::DrawVertex *points, int count, bool depthEnabled) override {
            assert(points != nullptr);
            assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

            glBindVertexArray(linePointVAO);
            glUseProgram(linePointProgram);

            glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
                               1, GL_FALSE, static_cast<float *>(&mvpMatrix[0][0]));

            if (depthEnabled) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }

            // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
            glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

            // Issue the draw call:
            glDrawArrays(GL_POINTS, 0, count);

            glUseProgram(0);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            checkGLError(__FILE__, __LINE__);
        }

        void drawLineList(const dd::DrawVertex *lines, int count, bool depthEnabled) override {
            assert(lines != nullptr);
            assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

            glBindVertexArray(linePointVAO);
            glUseProgram(linePointProgram);

            glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
                               1, GL_FALSE, static_cast<float *>(&mvpMatrix[0][0]));

            if (depthEnabled) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }

            // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
            glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), lines);

            // Issue the draw call:
            glDrawArrays(GL_LINES, 0, count);

            glUseProgram(0);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            checkGLError(__FILE__, __LINE__);
        }

        DDRenderInterfaceCoreGL()
                : mvpMatrix(glm::mat4(1.0f)), linePointProgram(0), linePointProgram_MvpMatrixLocation(-1),
                  linePointVAO(0), linePointVBO(0) {
            // Default OpenGL states:
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            // This has to be enabled since the point drawing shader will use gl_PointSize.
            glEnable(GL_PROGRAM_POINT_SIZE);

            setupShaderPrograms();
            setupVertexBuffers();

            std::printf("DDRenderInterfaceCoreGL ready!\n\n");
        }

        ~DDRenderInterfaceCoreGL() {
            glDeleteProgram(linePointProgram);
            glDeleteVertexArrays(1, &linePointVAO);
            glDeleteBuffers(1, &linePointVBO);
        }

        void setupShaderPrograms() {
            std::printf("> DDRenderInterfaceCoreGL::setupShaderPrograms()\n");

            //
            // Line/point drawing shader:
            //
            {
                GLuint linePointVS = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(linePointVS, 1, &linePointVertShaderSrc, nullptr);
                compileShader(linePointVS);

                GLint linePointFS = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(linePointFS, 1, &linePointFragShaderSrc, nullptr);
                compileShader(linePointFS);

                linePointProgram = glCreateProgram();
                glAttachShader(linePointProgram, linePointVS);
                glAttachShader(linePointProgram, linePointFS);

                glBindAttribLocation(linePointProgram, 0, "in_Position");
                glBindAttribLocation(linePointProgram, 1, "in_ColorPointSize");
                linkProgram(linePointProgram);

                linePointProgram_MvpMatrixLocation = glGetUniformLocation(linePointProgram, "u_MvpMatrix");
                if (linePointProgram_MvpMatrixLocation < 0) {
                }
                checkGLError(__FILE__, __LINE__);
            }
        }

        void setupVertexBuffers() {
            std::printf("> DDRenderInterfaceCoreGL::setupVertexBuffers()\n");

            //
            // Lines/points vertex buffer:
            //
            {
                glGenVertexArrays(1, &linePointVAO);
                glGenBuffers(1, &linePointVBO);
                checkGLError(__FILE__, __LINE__);

                glBindVertexArray(linePointVAO);
                glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);

                // RenderInterface will never be called with a batch larger than
                // DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
                glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr,
                             GL_STREAM_DRAW);
                checkGLError(__FILE__, __LINE__);

                // Set the vertex format expected by 3D points and lines:
                std::size_t offset = 0;

                glEnableVertexAttribArray(0); // in_Position (vec3)
                glVertexAttribPointer(
                        /* index     = */ 0,
                        /* size      = */ 3,
                        /* type      = */ GL_FLOAT,
                        /* normalize = */ GL_FALSE,
                        /* stride    = */ sizeof(dd::DrawVertex),
                        /* offset    = */ reinterpret_cast<void *>(offset));
                offset += sizeof(float) * 3;

                glEnableVertexAttribArray(1); // in_ColorPointSize (vec4)
                glVertexAttribPointer(
                        /* index     = */ 1,
                        /* size      = */ 4,
                        /* type      = */ GL_FLOAT,
                        /* normalize = */ GL_FALSE,
                        /* stride    = */ sizeof(dd::DrawVertex),
                        /* offset    = */ reinterpret_cast<void *>(offset));

                checkGLError(__FILE__, __LINE__);

                // VAOs can be a pain in the neck if left enabled...
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        }

        static GLuint handleToGL(dd::GlyphTextureHandle handle) {
            const std::size_t temp = reinterpret_cast<std::size_t>(handle);
            return static_cast<GLuint>(temp);
        }

        static dd::GlyphTextureHandle GLToHandle(const GLuint id) {
            const std::size_t temp = static_cast<std::size_t>(id);
            return reinterpret_cast<dd::GlyphTextureHandle>(temp);
        }

        static void checkGLError(const char *file, const int line) {
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
            }
        }

        static void compileShader(const GLuint shader) {
            glCompileShader(shader);
            checkGLError(__FILE__, __LINE__);

            GLint status;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            checkGLError(__FILE__, __LINE__);

            if (status == GL_FALSE) {
                GLchar strInfoLog[1024] = {0};
                glGetShaderInfoLog(shader, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
            }
        }

        static void linkProgram(const GLuint program) {
            glLinkProgram(program);
            checkGLError(__FILE__, __LINE__);

            GLint status;
            glGetProgramiv(program, GL_LINK_STATUS, &status);
            checkGLError(__FILE__, __LINE__);

            if (status == GL_FALSE) {
                GLchar strInfoLog[1024] = {0};
                glGetProgramInfoLog(program, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
            }
        }

        // The "model-view-projection" matrix for the scene.
        // In this demo, it consists of the camera's view and projection matrices only.
        glm::mat4 mvpMatrix;
        GLuint linePointProgram;
        GLint linePointProgram_MvpMatrixLocation;

    private:
        GLuint linePointVAO;
        GLuint linePointVBO;

        static const char *linePointVertShaderSrc;
        static const char *linePointFragShaderSrc;

}; // class DDRenderInterfaceCoreGL



#endif //SANDBOX_DEBUG_H
