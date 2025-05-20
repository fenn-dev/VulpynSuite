#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


GLuint LoadIconTexture(const char* filename, int* out_width, int* out_height)
{
    int channels;
    unsigned char* data = stbi_load(filename, out_width, out_height, &channels, 4); // force RGBA
    if (!data)
    {
        fprintf(stderr, "Failed to load image: %s\n", filename);
        return 0;
    }

    GLuint tex_id = 0;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // smoother scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *out_width, *out_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return tex_id;
}
