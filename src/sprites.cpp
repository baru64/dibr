#include <vector>
#include <algorithm>
#include <stdio.h>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "sprites.hpp"

SpriteGenerator :: SpriteGenerator(unsigned char* image_buffer, unsigned char* depth_map, int x, int y){
    //generate sprites
    for (int i=0; i < y; ++i) {
        for (int j=0; j < x; ++j) {
            Sprite new_sprite;
            new_sprite.size = 0.1f;
            new_sprite.r = image_buffer[3*x*i + 3*j + 0];
            new_sprite.g = image_buffer[3*x*i + 3*j + 1];
            new_sprite.b = image_buffer[3*x*i + 3*j + 2];
            new_sprite.a = 255;
            // new_sprite.pos = glm::vec3(j+(x/2), i+(y/2), depth_map[i*x+j]);
            new_sprite.pos = glm::vec3(
                j*0.1f-(x/2)*0.1f,
                (y*0.1f - (i*0.1f) + 0.1f) - (y/2)*0.1f,
                depth_map[i*x+j]*0.1f
            );
            
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
        color_buffer[4*i + 0] = (*it_sprite).r;
        color_buffer[4*i + 1] = (*it_sprite).g;
        color_buffer[4*i + 2] = (*it_sprite).b;
        color_buffer[4*i + 3] = (*it_sprite).a;
        ++it_sprite;
    }
}

void SpriteGenerator :: sortSprites() {
	std::sort(sprites_container.begin(), sprites_container.end());
}