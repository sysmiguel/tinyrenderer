#include "geometry.h"
#include "tgaimage.h"
#include <utility>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

const int width = 200;
const int height = 200;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
  int x0 = t0.x;
  int y0 = t0.y;
  int x1 = t1.x;
  int y1 = t1.y;

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

//line sweeping
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
 {
  //line(t0, t1, image, color);
  //line(t1, t2, image, color);
  //line(t2, t0, image, color);
  // sort for y values (t0,y>t1,y>t2,y)*/
  if (t0.y < t1.y)
    std::swap(t0, t1);
  if (t0.y < t2.y)
    std::swap(t0, t2);
  if (t1.y < t2.y)
    std::swap(t1, t2);

  // first part*/
  int triangleHeight1 = t0.y - t1.y;
  for (int i = triangleHeight1; i > 0; i--) {
    int y = i + t1.y;

    int x1_interpolated = t0.x + (t1.x - t0.x) * (y - t0.y) / (t1.y - t0.y);
    int x2_interpolated = t0.x + (t2.x - t0.x) * (y - t0.y) / (t2.y - t0.y);

    int lineWidth = std::abs(x2_interpolated - x1_interpolated);
    int xmin = std::min(x1_interpolated, x2_interpolated);
    for (int j = 0; j < lineWidth; j++) {
      int x = j + xmin;
      image.set(x, y, color);
    }
  }
  // second part
  int triangleHeight2 = t1.y - t2.y;
  for (int i = triangleHeight2; i > 0; i--) {
    int y = i + t2.y;

    int x1_interpolated = t0.x + (t2.x - t0.x) * (y - t0.y) / (t2.y - t0.y);
    int x2_interpolated = t1.x + (t2.x - t1.x) * (y - t1.y) / (t2.y - t1.y);

    int lineWidth = std::abs(x2_interpolated - x1_interpolated);
    int xmin = std::min(x1_interpolated, x2_interpolated);
    for (int j = 0; j < lineWidth; j++) {
      int x = j + xmin;
      image.set(x, y, color);
    }
  }
}

int main(int argc, char **argv) {
  TGAImage image(width, height, TGAImage::RGB);

  /*Model *model = new Model("./obj/african_head.obj");*/
  /*Model model("./obj/african_head.obj");*/
  // Wireframe
  /*for (int i = 0; i < model.nfaces(); i++) {*/
  /*  std::vector<int> face = model.face(i);*/
  /*  for (int j = 0; j < 3; j++) {*/
  /*    Vec3f v0 = model.vert(face[j]);*/
  /*    // wrapping around for last line (vertex 2 -> 0)*/
  /*    Vec3f v1 = model.vert(face[(j + 1) % 3]);*/
  /*    // converting from normalized coordinated to*/
  /*    // current space [-1, 1] -> [0, height/width]*/
  /*    int x0 = (v0.x + 1) * width / 2;*/
  /*    int y0 = (v0.y + 1) * height / 2;*/
  /*    int x1 = (v1.x + 1) * width / 2;*/
  /*    int y1 = (v1.y + 1) * height / 2;*/
  /*    line(x0, y0, x1, y1, image, white);*/
  /*  }*/
  /*}*/

  // triangles
  Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
  Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
  Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
  triangle(t0[0], t0[1], t0[2], image, red);
  triangle(t1[0], t1[1], t1[2], image, white);
  triangle(t2[0], t2[1], t2[2], image, green);
  image.flip_vertically();
  image.write_tga_file("output.tga");
  return 0;
}
