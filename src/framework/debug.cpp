#include <framework/debug.h>

const char * DDRenderInterfaceCoreGL::linePointVertShaderSrc = "\n"
                                                               "#version 150\n"
                                                               "\n"
                                                               "in vec3 in_Position;\n"
                                                               "in vec4 in_ColorPointSize;\n"
                                                               "\n"
                                                               "out vec4 v_Color;\n"
                                                               "uniform mat4 u_MvpMatrix;\n"
                                                               "\n"
                                                               "void main()\n"
                                                               "{\n"
                                                               "    gl_Position  = u_MvpMatrix * vec4(in_Position, 1.0);\n"
                                                               "    gl_PointSize = in_ColorPointSize.w;\n"
                                                               "    v_Color      = vec4(in_ColorPointSize.xyz, 1.0);\n"
                                                               "}\n";

const char * DDRenderInterfaceCoreGL::linePointFragShaderSrc = "\n"
                                                               "#version 150\n"
                                                               "\n"
                                                               "in  vec4 v_Color;\n"
                                                               "out vec4 out_FragColor;\n"
                                                               "\n"
                                                               "void main()\n"
                                                               "{\n"
                                                               "    out_FragColor = v_Color;\n"
                                                               "}\n";

