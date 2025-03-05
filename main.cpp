#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <utility>

constexpr TGAColor white = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

const int width = 1200;
const int height = 1200;

void line(int ax, int ay, int bx, int by, TGAImage &image, TGAColor color) {

  bool steep = false;
  if (std::abs(bx - ax) < std::abs(by - ay)) {
    std::swap(ax, ay);
    std::swap(bx, by);
    steep = true;
  }
  if (bx < ax) {
    std::swap(ax, bx);
    std::swap(ay, by);
  }
  // Before we incremented error by derror=dy/dx
  // then we checked if error > 0.5 and decremented by 1 (error -= 1.)
  // To avoid floating point division we can multiply by 2
  // error > 0.5 -> error2 > 1 (decrement by 2 now -> error2 -= 2.)
  // derror = dy/dx -> derror2 = 2dy/dx
  // Now we can multiply by dx to get rid of division
  // derror2 = 2dy/dx -> derror2 = 2dy
  // error2 > 1*dx (error2 -= 2*dx)
  float dx = bx - ax;
  float dy = by - ay;
  float error2 = 0.;
  float derror2 = std::abs(dy) * 2;
  int y = ay;
  for (int x = ax; x <= bx; x++) {
    if (steep) {
      image.set(y, x, color);
    } else {
      image.set(x, y, color);
    }
    error2 += derror2;
    if (error2 > dx) {
      error2 -= dx * 2;
      y += (by > ay ? 1 : -1);
    }
  }
}

std::tuple<int, int> project(vec3 v) {
  return {(v.x + 1.) * width / 2, (v.y + 1.) * height / 2};
}

// line sweeping
void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &image,
              TGAColor color) {
  // line(t0, t1, image, color);
  // line(t1, t2, image, color);
  // line(t2, t0, image, color);
  //  sort for y values (t0,y>t1,y>t2,y)*/
  if (ay < by) {
    std::swap(ax, bx);
    std::swap(ay, by);
  }
  if (ay < cy) {
    std::swap(ax, cx);
    std::swap(ay, cy);
  }
  if (by < cy) {
    std::swap(bx, cx);
    std::swap(by, cy);
  }

  // first part*/
  int triangleHeight1 = ay - by;
  for (int i = triangleHeight1; i > 0; i--) {
    int y = i + by;

    int x1_interpolated = ax + (bx - ax) * (y - ay) / (by - ay);
    int x2_interpolated = ax + (cx - ax) * (y - ay) / (cy - ay);

    int lineWidth = std::abs(x2_interpolated - x1_interpolated);
    int xmin = std::min(x1_interpolated, x2_interpolated);
    for (int j = 0; j < lineWidth; j++) {
      int x = j + xmin;
      image.set(x, y, color);
    }
  }
  // second part
  int triangleHeight2 = by - cy;
  for (int i = triangleHeight2; i > 0; i--) {
    int y = i + cy;

    int x1_interpolated = ax + (cx - ax) * (y - ay) / (cy - ay);
    int x2_interpolated = bx + (cx - bx) * (y - by) / (cy - by);

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

  Model model("./obj/anime.obj");
  // wireframe
  /*
for (int i = 0; i < model.nfaces(); i++) {
auto [ax, ay] = project(model.vert(i, 0));
auto [bx, by] = project(model.vert(i, 1));
auto [cx, cy] = project(model.vert(i, 2));
line(ax, ay, bx, by, image, red);
line(bx, by, cx, cy, image, red);
line(cx, cy, ax, ay, image, red);
}

for (int i = 0; i < model.nverts(); i++) {

vec3 v = model.vert(i);
auto [x, y] = project(v);
image.set(x, y, white);
}
  */
  // Rasterization

  for (int i = 0; i < model.nfaces(); i++) {

    auto [ax, ay] = project(model.vert(i, 0));
    auto [bx, by] = project(model.vert(i, 1));
    auto [cx, cy] = project(model.vert(i, 2));

    constexpr TGAColor green = {0, 255, 0, 255};
    TGAColor randomColor = {static_cast<std::uint8_t>(rand() % 255),
                            static_cast<std::uint8_t>(rand() % 255),
                            static_cast<std::uint8_t>(rand() % 255),
                            static_cast<std::uint8_t>(255)};

    triangle(ax, ay, bx, by, cx, cy, image, randomColor);
  }

  image.write_tga_file("output.tga");
  return 0;
}
