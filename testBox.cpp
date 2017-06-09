#include<SDL.h>
#include"isom_DotSet.hpp"
#include"isom_LineContext.hpp"
#include"isom_screen.hpp"
#include"isom_image.hpp"
#include"isom_point.hpp"
#include"isom_plane.hpp"
#include"isom_object.hpp"
#include"isom_renderer.hpp"
#include<random>


#ifdef EMSCRIPTEN
#include"emscripten.h"
#endif


namespace{


bool  needed_to_redraw = true;


constexpr int  sz = 64;


std::default_random_engine  eng;
std::uniform_real_distribution<>  dist(-100,100);


Renderer
renderer(screen::width,screen::height);


Image  a_texture;
Image  b_texture;


Object  obj;


Transformer  tr;


void
render()
{
    if(needed_to_redraw)
    {
      screen::lock();

      screen::clear();

      renderer.clear();

       
      Object  o = obj;

      o.transform(tr);

      o.render(renderer);


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

          bool  shifting = (SDL_GetModState()&KMOD_SHIFT);

          auto  o = tr.get_angle();

            switch(evt.key.keysym.sym)
            {
          case(SDLK_LEFT ):                                       {o.x_degree -= step;}break;
          case(SDLK_RIGHT):                                       {o.x_degree += step;}break;
          case(SDLK_UP   ): if(shifting){o.z_degree -= step;} else{o.y_degree += step;}break;
          case(SDLK_DOWN ): if(shifting){o.z_degree += step;} else{o.y_degree -= step;}break;
          case(SDLK_SPACE):
              break;
          case(SDLK_1): screen::save_as_bmp();break;
            }


          tr.change_angle(o);
        }
    }


  render();
}


}




int
main(int  argc, char**  argv)
{
  screen::open();

  a_texture.open("expandmetal.png");
  b_texture.open("lena_std.png");

  a_texture.set_colorkey(0,0,0);

  std::random_device  rdev;

  eng = std::default_random_engine(rdev());


  obj = Object(ObjectList());

  Box  box;

  box.build(Point(0,0,0),80,80,40);

  box.get_bottom().change_texture(&a_texture,Rect(0,0,64,64));
  box.get_left().change_texture(&a_texture,Rect(0,0,64,64));
  box.get_back().change_texture(&a_texture,Rect(0,0,64,64));

  box.get_top().change_texture(&a_texture,Rect(0,0,64,64));
  box.get_right().change_texture(&a_texture,Rect(0,0,64,64));
  box.get_front().change_texture(&a_texture,Rect(0,0,64,64));


  obj->object_list.emplace_back(std::move(box));


  Polygon  poly(Dot(Point( 0, 0, 0),Color(0xFF,0   ,0xFF,0xFF)),
                Dot(Point(80,40, 0),Color(0x00,0xFF,0xFF,0xFF)),
                Dot(Point( 0,80,40),Color(0xFF,0   ,0xFF,0xFF)));

  obj->object_list.emplace_back(std::move(poly));

  

  tr.change_offset(sz*2,-sz*2,0);
  tr.change_center(0,0,0);

  tr.set_translation_flag();
  tr.set_rotation_flag();

  render();


if(0)
{
    for(int  i = 0;  i <= 360;  i += 5)
    {
      printf("%10f,//%3d\n",std::sin(to_radian(i)),i);
    }


  printf("-----\n");

    for(int  i = 0;  i <= 360;  i += 5)
    {
      printf("%10f,//%3d\n",std::cos(to_radian(i)),i);
    }
  fflush(stdout);
}


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


	
