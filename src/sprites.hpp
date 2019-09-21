#ifndef SPRITES_HPP
#define SPRITES_HPP

struct Sprite{
	glm::vec3 pos;
	unsigned char r,g,b,a; // Color
	float size;
	float camera_distance; 

	bool operator<(const Sprite& that) const {
		// Sort in reverse order : far sprites drawn first.
		return this->camera_distance > that.camera_distance;
	}
};

class SpriteGenerator {
    public:
        int sprite_count;

        SpriteGenerator(char* image_buffer, char* depth_map, int x, int y);
        void FillPositionSizeBuffer(GLfloat* position_size_buffer);
        void FillColorBuffer(GLubyte* color_buffer);
        void SortSprites();
        void UpdateCameraDistance(glm::vec3 camera_position);
    private:
        std::vector<Sprite> sprites_container;
};

#endif