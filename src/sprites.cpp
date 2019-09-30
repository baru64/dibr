#include <vector>
#include <algorithm>
#include <stdio.h>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "sprites.hpp"

SpriteGenerator :: SpriteGenerator( unsigned char* image_buffer,
                                    unsigned char* depth_map,
                                    int x, int y,
                                    float depth_scale,
                                    float background_filter,
                                    bool unproject ) {
    //generate sprites
    for (int i=0; i < y; ++i) {
        for (int j=0; j < x; ++j) {
            if ((depth_map[i*x+j] * depth_scale) < background_filter) {
                // skip background
                continue;
            }
            Sprite new_sprite;
            new_sprite.size = 0.1f;
            new_sprite.r = image_buffer[3*x*i + 3*j + 0];
            new_sprite.g = image_buffer[3*x*i + 3*j + 1];
            new_sprite.b = image_buffer[3*x*i + 3*j + 2];
            new_sprite.a = 255;
            // new_sprite.pos = glm::vec3(j+(x/2), i+(y/2), depth_map[i*x+j]);
            if (unproject) {
                new_sprite.size = 0.2f;
                glm::vec4 new_pos = glm::vec4(
                    j*0.1f-(x/2)*0.1f,
                    (y*0.1f - (i*0.1f) + 0.1f) - (y/2)*0.1f,
                    depth_map[i*x+j] * depth_scale,
                    1
                );
                new_pos = glm::inverse(
                    glm::lookAt(
                        glm::vec3(10,0,60),
                        glm::vec3(0,0,0),
                        glm::vec3(0,1,0)
                    )
                ) * new_pos;
                new_pos = glm::inverse(glm::perspective(glm::radians(45.0f),
                                4.0f / 3.0f, 0.1f, 100.f)) * new_pos;
                new_sprite.pos = glm::vec3(
                    new_pos.x, new_pos.y, new_pos.z
                );
            } else {
                new_sprite.pos = glm::vec3(
                    j*0.1f-(x/2)*0.1f,
                    (y*0.1f - (i*0.1f) + 0.1f) - (y/2)*0.1f,
                    depth_map[i*x+j] * depth_scale
                );
            }

            new_sprite.selected = false;
            sprites_container.push_back(new_sprite);
        }
    }
    sprite_count = sprites_container.size();
}

void SpriteGenerator :: updateCameraDistance(glm::vec3 camera_position) {
    for (int i = 0; i < sprites_container.size(); ++i) {
        sprites_container[i].camera_distance =
            glm::length2(sprites_container[i].pos - camera_position);
    }
}

void SpriteGenerator :: fillPositionSizeBuffer(GLfloat* position_size_buffer) {
    std::vector<Sprite>::iterator it_sprite = sprites_container.begin();

    for (int i = 0; i < sprite_count; ++i) {
        position_size_buffer[4*i + 0] = (*it_sprite).pos.x;
        position_size_buffer[4*i + 1] = (*it_sprite).pos.y;
        position_size_buffer[4*i + 2] = (*it_sprite).pos.z;
        position_size_buffer[4*i + 3] = (*it_sprite).size;
        ++it_sprite;
    }
}

void SpriteGenerator :: fillColorBuffer(GLubyte* color_buffer) {
    std::vector<Sprite>::iterator it_sprite = sprites_container.begin();

    for (int i = 0; i < sprite_count; ++i) {
        if (it_sprite->selected) {
            color_buffer[4*i + 0] = 255;
            color_buffer[4*i + 1] = 0;
            color_buffer[4*i + 2] = 0;
            color_buffer[4*i + 3] = 255;
        } else {
            color_buffer[4*i + 0] = (*it_sprite).r;
            color_buffer[4*i + 1] = (*it_sprite).g;
            color_buffer[4*i + 2] = (*it_sprite).b;
            color_buffer[4*i + 3] = (*it_sprite).a;
        }
        ++it_sprite;
    }
}

void SpriteGenerator :: sortSprites() {
	std::sort(sprites_container.begin(), sprites_container.end());
}

void SpriteGenerator :: select(glm::vec3 obj_position) {
    std::vector<Sprite>::reverse_iterator it_sprite = sprites_container.rbegin();
    for (int i = 0; i < sprite_count; ++i) {
        if (
            (   obj_position.x - 0.1f < it_sprite->pos.x
            && obj_position.x + 0.1f > it_sprite->pos.x )
                &&
            (   obj_position.y - 0.1f < it_sprite->pos.y
            && obj_position.y + 0.1f > it_sprite->pos.y )
                &&
            (   obj_position.z - 0.1f < it_sprite->pos.z
            && obj_position.z + 0.1f > it_sprite->pos.z )
        ) {
            it_sprite->selected = true;
        }
        ++it_sprite;
    }
}

void SpriteGenerator :: removeSelected() {
    for(
        std::vector<Sprite>::iterator i = sprites_container.begin();
        i != sprites_container.end();
        ++i
    ) {
        if (i->selected) sprites_container.erase(i);
    }
}

void SpriteGenerator :: cancelSelection() {
    for(
        std::vector<Sprite>::iterator i = sprites_container.begin();
        i != sprites_container.end();
        ++i
    ) {
        if (i->selected) i->selected = false;
    }
}