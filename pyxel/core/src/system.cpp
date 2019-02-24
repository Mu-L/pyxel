#include "pyxelcore/system.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>

namespace pyxelcore {

System::System(Graphics *graphics, int width, int height, char *caption,
               int scale, int *palette, int fps, int border_width,
               int border_color) {
  graphics_ = graphics;

  width_ = width;
  height_ = height;
  caption_ = std::string(caption);
  fps_ = fps;
  border_width_ = border_width;
  border_color_ = border_color;

  for (int i = 0; i < 16; i++) {
    palette_[i] = palette[i];
  }

  SDL_Init(SDL_INIT_VIDEO);

  window_ = SDL_CreateWindow(caption, SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, width, height, 0);
  renderer_ = SDL_CreateRenderer(window_, -1, 0);

  int flags = IMG_INIT_PNG;
  int initted = IMG_Init(flags);
  if ((initted & flags) != flags) {
    printf("IMG_Init: Failed to init required jpg and png support!\n");
    printf("IMG_Init: %s\n", IMG_GetError());
    return;
  }

  SDL_Surface *image = IMG_Load("../examples/assets/pyxel_logo_152x64.png");

  if (!image) {
    printf("IMG_Load: %s\n", IMG_GetError());
    return;
  }

  temp_texture_ = SDL_CreateTextureFromSurface(renderer_, image);

  screen_texture_ =
      SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGB888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);
}

System::~System() {}

void System::Run(void (*update)(), void (*draw)()) {
  SDL_Event ev;

  while (1) {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT)
        return;
    }

    graphics_->Cls(8);
    UpdateScreenTexture();
    SDL_RenderCopy(renderer_, screen_texture_, NULL, NULL);

    int iw, ih;
    SDL_QueryTexture(temp_texture_, NULL, NULL, &iw, &ih);
    SDL_Rect image_rect = (SDL_Rect){0, 0, iw, ih};
    SDL_Rect draw_rect = (SDL_Rect){50, 50, iw, ih};
    SDL_RenderCopy(renderer_, temp_texture_, &image_rect, &draw_rect);

    SDL_SetRenderDrawColor(renderer_, 255, 255, 0, 255);
    SDL_RenderDrawLine(renderer_, 10, 10, 400, 400);

    SDL_RenderPresent(renderer_);
  }
}

void System::UpdateScreenTexture() {
  int *pixel;
  int pitch;
  int *framebuffer = graphics_->GetFramebuffer();
  size_t size = width_ * height_;

  SDL_LockTexture(screen_texture_, NULL, (void **)&pixel, &pitch);

  for (size_t i = 0; i < size; i++) {
    pixel[i] = palette_[framebuffer[i]];
  }

  SDL_UnlockTexture(screen_texture_);
}

} // namespace pyxelcore