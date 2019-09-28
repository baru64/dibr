#ifndef SPRITES_HPP
#define SPRITES_HPP

#include <vector>

struct Sprite{
	glm::vec3 pos;
	unsigned char r,g,b,a; // Color
	float size;
	float camera_distance; 
    bool selected;

	bool operator<(const Sprite& that) const {
		// Sort in reverse order : far sprites drawn first.
		return this->camera_distance > that.camera_distance;
	}
};

class SpriteGenerator {
    public:
        int sprite_count;

        SpriteGenerator(
            unsigned char* image_buffer,
            unsigned char* depth_map,
            int x, int y,
            float depth_scale,
            float background_filter
        );
        void fillPositionSizeBuffer(GLfloat* position_size_buffer);
        void fillColorBuffer(GLubyte* color_buffer);
        void sortSprites();
        void updateCameraDistance(glm::vec3 camera_position);
        void select(glm::vec3 obj_position);
        void removeSelected();
        void cancelSelection();
    private:
        std::vector<Sprite> sprites_container;
};

#endif