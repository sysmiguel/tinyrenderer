#include "model.h"
#include "tgaimage.h"
#include <utility>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
  bool steep = false;
  if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }
  if (x1 < x0) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  // Before we incremented error by derror=dy/dx
  // then we checked if error > 0.5 and decremented by 1 (error -= 1.)
  // To avoid floating point division we can multiply by 2
  // error > 0.5 -> error2 > 1 (decrement by 2 now -> error2 -= 2.)
  // derror = dy/dx -> derror2 = 2dy/dx
  // Now we can multiply by dx to get rid of division
  // derror2 = 2dy/dx -> derror2 = 2dy
  // error2 > 1*dx (error2 -= 2*dx)
  float dx = x1 - x0;
  float dy = y1 - y0;
  float error2 = 0.;
  float derror2 = std::abs(dy) * 2;
  int y = y0;
  for (int x = x0; x <= x1; x++) {
    if (steep) {
      image.set(y, x, color);
    } else {
      image.set(x, y, color);
    }
    error2 += derror2;
    if (error2 > dx) {
      error2 -= dx * 2;
      y += (y1 > y0 ? 1 : -1);
    }
  }
}

int main(int argc, char **argv) {
  int width = 1000;
  int height = 1000;
  TGAImage image(width, height, TGAImage::RGB);

  Model *model = new Model("./obj/african_head.obj");

  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i);
    for (int j = 0; j < 3; j++) {
      Vec3f v0 = model->vert(face[j]);
      // wrapping around for last line (vertex 2 -> 0)
      Vec3f v1 = model->vert(face[(j + 1) % 3]);
      // converting from normalized coordinated to
      // current space [-1, 1] -> [0, height/width]
      int x0 = (v0.x + 1) * width / 2;
      int y0 = (v0.y + 1) * height / 2;
      int x1 = (v1.x + 1) * width / 2;
      int y1 = (v1.y + 1) * height / 2;
      line(x0, y0, x1, y1, image, white);
    }
  }

  image.flip_vertically();
  image.write_tga_file("output.tga");
  return 0;
}
