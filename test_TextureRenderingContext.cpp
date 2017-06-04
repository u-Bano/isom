#include<SDL.h>
#include"isom_DotSet.hpp"
#include"isom_LineContext.hpp"
#include"isom_screen.hpp"
#include"isom_image.hpp"
#include"isom_point.hpp"
#include"isom_plane.hpp"
#include"isom_renderer.hpp"


#ifdef EMSCRIPTEN
#include"emscripten.h"
#endif


namespace{


bool  needed_to_redraw = true;


int  x_degree;
int  y_degree;
int  z_degree;

Renderer
renderer(screen::width,screen::height);


constexpr int  sz = 128;


Image
texture;


Plane  base_plane(Point(0,sz,0),Point(sz,sz,0),Point(sz,0,0),Point(0,0,0));
Plane       plane;


DotSet
dotset;

DotSet
nega_dotset;

std::vector<TextureRenderingContext>
ctx_stack;


void
render()
{
    if(needed_to_redraw)
    {
      screen::lock();

      screen::clear();

      renderer.clear();


      renderer.render_dotset(dotset);

      plane.render_wire(renderer);

      renderer.render_image(texture,nullptr,0,0,0);

      renderer.render_dotset(nega_dotset);

      screen::put_renderer(renderer,0,0);


      screen::unlock();
      screen::update();

      needed_to_redraw = false;
    }
}


void
main_loop()
{
  static SDL_Event  evt;

    while(SDL_PollEvent(&evt))
    {
        switch(evt.type)
        {
#ifndef EMSCRIPTEN
      case(SDL_DROPFILE):
          break;
      case(SDL_WINDOWEVENT):
            if(evt.window.event == SDL_WINDOWEVENT_EXPOSED)
            {
              needed_to_redraw = true;
            }
          break;
      case(SDL_QUIT):
          screen::close();
          quick_exit(EXIT_SUCCESS);
          break;
#endif
      case(SDL_KEYDOWN):
          needed_to_redraw = true;

          constexpr double  step = 5;

          bool  flag = false;

          bool  shiting = (SDL_GetModState()&KMOD_SHIFT);

            switch(evt.key.keysym.sym)
            {
          case(SDLK_LEFT ):                                 {x_degree -= step;}break;
          case(SDLK_RIGHT):                                 {x_degree += step;}break;
          case(SDLK_UP   ): if(shiting){z_degree += step;} else{y_degree += step;}break;
          case(SDLK_DOWN ): if(shiting){z_degree -= step;} else{y_degree -= step;}break;
          case(SDLK_SPACE):
              flag = true;
              break;
          case(SDLK_1): screen::save_as_bmp();break;
            }


          plane = base_plane;

          plane.x_degree += x_degree;
          plane.y_degree += y_degree;
          plane.z_degree += z_degree;

          plane.update();

            if(flag)
            {
                   dotset.clear();
              nega_dotset.clear();

              ctx_stack.clear();

              constexpr Rect  rect(0,0,sz,sz);

              ctx_stack.emplace_back(plane.make_texture_rendering_context(0,texture,rect));
              ctx_stack.emplace_back(plane.make_texture_rendering_context(1,texture,rect));
            }
        }
    }


static uint32_t  last;

auto  now = SDL_GetTicks();

  if(now >= last+40)
  {
    last = now;

      if(ctx_stack.size())
      {
        auto&  ctx = ctx_stack.back();

        int  n = 200;

          while(n--)
          {
            auto&  p = ctx.get_plotter();
            auto&  m = ctx.get_mapper();

                 dotset.emplace_back(p.get_x(), p.get_y(),p.get_z(),ctx.get_color());
            nega_dotset.emplace_back(m.get_u(),-m.get_v(),        1,        Color());

              if(ctx.is_finished())
              {
                ctx_stack.pop_back();

                break;
              }

            else
              {
                ctx.step();
              }
          }


        needed_to_redraw = true;
      }
  }


  render();
}


}




int
main(int  argc, char**  argv)
{
  screen::open();

  texture.open("lena_std.png");

  base_plane.offset.assign(200,-400,0);

  plane = base_plane;

  plane.update();


  render();

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(main_loop,-1,false);
#else
    for(;;)
    {
      main_loop();

      SDL_Delay(20);
    }
#endif


  return 0;
}


	

