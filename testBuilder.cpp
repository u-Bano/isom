#include<SDL.h>
#include<list>
#include"isom_LineContext.hpp"
#include"isom_screen.hpp"
#include"isom_image.hpp"
#include"isom_point.hpp"
#include"isom_object.hpp"
#include"isom_renderer.hpp"
#include"isom_load.hpp"
#include"isom_mouse.hpp"
#include"libjson/json_FileBuffer.hpp"
#include"libjson/json_stream.hpp"


#ifdef EMSCRIPTEN
#include"emscripten.h"
#endif


namespace{


bool  needed_to_redraw = true;

Renderer  main_renderer(   0,0,screen::width,screen::height);


Image  texture;


Object  standard;
Object    stages;

int  target_index;

std::vector<Object>::const_iterator  target;

Transformer   view_tr;
Transformer  stage_tr;

Mouse  mouse;


void
render_main()
{
  main_renderer.clear();

  standard.render(main_renderer,{&stage_tr,&view_tr});

    if(target != stages.get_children().cend())
    {
      Formatted  fmt;

      main_renderer.draw_ascii(fmt("%2d %s",target_index,target->get_name().data()),white,0,0);

      target->render(main_renderer,{&stage_tr,&view_tr});
    }
}


void
render()
{
    if(needed_to_redraw)
    {
      screen::lock();

      screen::clear();

      render_main();

      screen::put_renderer(main_renderer,0,0);

      screen::unlock();
      screen::update();


      needed_to_redraw = false;
    }
}


void
load(const std::string&  obj_s, const std::string&  stg_s)
{
  Object  o;

    try
    {
      auto  object_o = load_object(         obj_s);
                   o =  load_stage(object_o,stg_s);
    }


    catch(libjson::Stream&  s)
    {
      s.print();

      return;
    }


  stages = std::move(o);

  auto&  arr = stages.get_children();

  target = arr.cbegin();

    if(target_index < arr.size())
    {
        for(int  i = 0;  i < target_index;  ++i)
        {
          ++target;
        }
    }

  else
    {
      target_index = 0;
    }


  needed_to_redraw = true;
}


void
process_keydown(int  k)
{
    if(k == SDLK_LEFT)
    {
        if(target_index)
        {
          --target      ;
          --target_index;

          needed_to_redraw = true;
        }
    }

  else
    if(k == SDLK_RIGHT)
    {
      int  n = stages.get_children().size();

        if(target_index < (n-1))
        {
          ++target      ;
          ++target_index;

          needed_to_redraw = true;
        }
    }

#ifndef EMSCRIPTEN
  else
    if(k == SDLK_SPACE)
    {
      FileBuffer  object_fbuf(FilePath("../object.json"));
      FileBuffer   stage_fbuf(FilePath("../stage.json"));

      load(object_fbuf.get_content(),stage_fbuf.get_content());
    }
#endif
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
          process_keydown(evt.key.keysym.sym);
          break;
      case(SDL_MOUSEBUTTONDOWN):
          mouse.x = evt.button.x;
          mouse.y = evt.button.y;

               if(evt.button.button == SDL_BUTTON_LEFT ){mouse.left  = 1;}
          else if(evt.button.button == SDL_BUTTON_RIGHT){mouse.right = 1;}

          mouse.modified = 1;
          break;
      case(SDL_MOUSEBUTTONUP):
          mouse.x = evt.button.x;
          mouse.y = evt.button.y;

               if(evt.button.button == SDL_BUTTON_LEFT ){mouse.left  = 0;}
          else if(evt.button.button == SDL_BUTTON_RIGHT){mouse.right = 0;}

          mouse.modified = 1;
          break;
      case(SDL_MOUSEMOTION):
          mouse.x = evt.button.x;
          mouse.y = evt.button.y;

          mouse.left  = evt.motion.state&SDL_BUTTON_LMASK;
          mouse.right = evt.motion.state&SDL_BUTTON_RMASK;

          mouse.modified = 1;
          break;
        }
    }


    if(mouse.modified)
    {
      static int  x_prev;
      static int  y_prev;

        if((mouse.x != x_prev) ||
           (mouse.y != y_prev))
        {
            if(mouse.left)
            {
              int  x = main_renderer.get_x_base()+(x_prev-mouse.x);
              int  y = main_renderer.get_y_base()-(y_prev-mouse.y);

              main_renderer.change_base_point(x,y);

              needed_to_redraw = true;
            }


          x_prev = mouse.x;
          y_prev = mouse.y;
        }


      mouse.modified = 0;
    }


  render();
}


void
make_standard()
{
  int  l = 200;

  standard.push(Line(Dot(Point(-l,   0,   0),black),
                     Dot(Point( 0,   0,   0),red)));
  standard.push(Line(Dot(Point( 0,   0,   0),red),
                     Dot(Point( l,   0,   0),white)));

  standard.push(Line(Dot(Point(   0,-l,   0),black),
                     Dot(Point(   0, 0,   0),green)));
  standard.push(Line(Dot(Point(   0, 0,   0),green),
                     Dot(Point(   0, l,   0),white)));

  standard.push(Line(Dot(Point(   0,   0,-l),black),
                     Dot(Point(   0,   0, 0),blue)));
  standard.push(Line(Dot(Point(   0,   0, 0),blue),
                     Dot(Point(   0,   0, l),white)));
}


}




int
main(int  argc, char**  argv)
{
  screen::open();

  texture.open("texture.png");

  Polygon::texture_image = &texture;

  Renderer::default_lightset.directional.vector = Vector(-1,0,-1);
  Renderer::default_lightset.directional.color  = Color(0x7F,0x7F,0x7F,0xFF);
  Renderer::default_lightset.ambient.color      = Color(0x3F,0x3F,0x3F,0xFF);

  view_tr.change_angle(315,-35,-30);
  view_tr.set_rotation_flag();
  stage_tr.set_rotation_flag();

  Renderer::default_lightset.transform(view_tr);

  make_standard();

#ifdef EMSCRIPTEN
const std::string  obj_s = 
R"(
{
{
  "objects":{
    "expressions":[
      "u = 48"
      "t = 24"
    ],
    "Floor":{
      "elements":{
        "":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"0",  "u":"0","v":"0"},
            "b":{"x":"0","y":"0","z":"u",  "u":"0","v":"0"},
            "c":{"x":"u","y":"0","z":"u",  "u":"0","v":"0"},
            "d":{"x":"u","y":"0","z":"0",  "u":"0","v":"0"},
          }
        }
      }
    },


    "Pillar":{
      "elements":{
        "front":{
          "tetragon":{
            "a":{"x":"0","y":"  0","z":"u"},
            "b":{"x":"u","y":"  0","z":"u"},
            "c":{"x":"u","y":"u*3","z":"u"},
            "d":{"x":"0","y":"u*3","z":"u"},
          }
        },

        "back":{
          "tetragon":{
            "a":{"x":"0","y":"  0","z":"0"},
            "b":{"x":"0","y":"u*3","z":"0"},
            "c":{"x":"u","y":"u*3","z":"0"},
            "d":{"x":"u","y":"  0","z":"0"},
          }
        },

        "left":{
          "tetragon":{
            "a":{"x":"0","y":"  0","z":"0"},
            "b":{"x":"0","y":"  0","z":"u"},
            "c":{"x":"0","y":"u*3","z":"u"},
            "d":{"x":"0","y":"u*3","z":"0"},
          }
        },

        "right":{
          "tetragon":{
            "a":{"x":"u","y":"  0","z":"0"},
            "b":{"x":"u","y":"u*3","z":"0"},
            "c":{"x":"u","y":"u*3","z":"u"},
            "d":{"x":"u","y":"  0","z":"u"},
          }
        },


        "bottom":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"0"},
            "b":{"x":"u","y":"0","z":"0"},
            "c":{"x":"u","y":"0","z":"u"},
            "d":{"x":"0","y":"0","z":"u"},
          }
        },

        "top":{
          "tetragon":{
            "a":{"x":"0","y":"u*3","z":"0"},
            "b":{"x":"0","y":"u*3","z":"u"},
            "c":{"x":"u","y":"u*3","z":"u"},
            "d":{"x":"u","y":"u*3","z":"0"},
          }
        }
      }
    },


    "Door":{
      "elements":{
        "":{
          "tetragon":{
            "a":{"x":"0","y":"  0","z":"0",  "u":"t*4","v":"t"},
            "b":{"x":"u","y":"  0","z":"0",  "u":"t*5","v":"t"},
            "c":{"x":"u","y":"u*2","z":"0",  "u":"t*5","v":"0"},
            "d":{"x":"0","y":"u*2","z":"0",  "u":"t*4","v":"0"},
          }
        }
      }
    },


    "Block":{
      "elements":{
        "front":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"u",  "u":"t*2","v":"t"},
            "b":{"x":"u","y":"0","z":"u",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"u","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"0","y":"u","z":"u",  "u":"t*2","v":"0"},
          }
        },

        "back":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"0","y":"u","z":"0",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"u","z":"0",  "u":"t*3","v":"0"},
            "d":{"x":"u","y":"0","z":"0",  "u":"t*2","v":"0"},
          }
        },

        "left":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"0","y":"0","z":"u",  "u":"t*3","v":"t"},
            "c":{"x":"0","y":"u","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"0","y":"u","z":"0",  "u":"t*2","v":"0"},
          }
        },

        "right":{
          "tetragon":{
            "a":{"x":"u","y":"0","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"u","y":"u","z":"0",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"u","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"u","y":"0","z":"u",  "u":"t*2","v":"0"},
          }
        },


        "bottom":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"u","y":"0","z":"0",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"0","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"0","y":"0","z":"u",  "u":"t*2","v":"0"},
          }
        },

        "top":{
          "tetragon":{
            "a":{"x":"0","y":"u","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"0","y":"u","z":"u",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"u","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"u","y":"u","z":"0",  "u":"t*2","v":"0"},
          }
        }
      }
    },


    "Stairs":{
      "elements":{
        "front":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"u",  "u":"t*2","v":"t"},
            "b":{"x":"u","y":"0","z":"u",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"u","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"0","y":"u","z":"u",  "u":"t*2","v":"0"},
          }
        },

        "back":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"0","y":"u","z":"0",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"u","z":"0",  "u":"t*3","v":"0"},
            "d":{"x":"u","y":"0","z":"0",  "u":"t*2","v":"0"},
          }
        },

        "left":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"0","y":"0","z":"u",  "u":"t*3","v":"t"},
            "c":{"x":"0","y":"u","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"0","y":"u","z":"0",  "u":"t*2","v":"0"},
          }
        },

        "right":{
          "tetragon":{
            "a":{"x":"u","y":"0","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"u","y":"u","z":"0",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"u","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"u","y":"0","z":"u",  "u":"t*2","v":"0"},
          }
        },


        "bottom":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"u","y":"0","z":"0",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"0","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"0","y":"0","z":"u",  "u":"t*2","v":"0"},
          }
        },

        "top":{
          "tetragon":{
            "a":{"x":"0","y":"u","z":"0",  "u":"t*2","v":"t"},
            "b":{"x":"0","y":"u","z":"u",  "u":"t*3","v":"t"},
            "c":{"x":"u","y":"u","z":"u",  "u":"t*3","v":"0"},
            "d":{"x":"u","y":"u","z":"0",  "u":"t*2","v":"0"},
          }
        },

        "stairs":{
          "tetragon":{
            "a":{"x":"0","y":"0","z":"u*2",  "u":"t*3","v":"t"},
            "b":{"x":"u","y":"0","z":"u*2",  "u":"t*4","v":"t"},
            "c":{"x":"u","y":"u","z":"u  ",  "u":"t*4","v":"0"},
            "d":{"x":"0","y":"u","z":"u  ",  "u":"t*3","v":"0"},
          }
        }
      }
    }
  }
}



)";


const std::string  stg_s = 
R"(
{
  "expressions":[
    "u = 48",
  ],

  "stages":{
    "House":{
      "objects":{
        "bottom floor":{
          "Floor":{
            "offset":"0,0,u",
            "increment":"u,0,0",
            "count":8,
          },

          "Floor":{
            "offset":"0,0,u*2",
            "increment":"u,0,0",
            "count":8,
          },

          "Floor":{
            "offset":"0,0,u*3",
            "increment":"u,0,0",
            "count":8,
          },

          "Floor":{
            "offset":"0,0,u*4",
            "increment":"u,0,0",
            "count":8,
          },

          "Floor":{
            "offset":"0,0,u*5",
            "increment":"u,0,0",
            "count":8,
          },

          "Floor":{
            "offset":"0,0,u*6",
            "increment":"u,0,0",
            "count":8,
          },

          "Floor":{
            "offset":"0,0,u*7",
            "increment":"u,0,0",
            "count":8,
          },

          "Floor":{
            "offset":"0,0,u*8",
            "increment":"u,0,0",
            "count":8,
          }
        },

        "wall":{
          "Pillar":{
            "offset":"0,0,0",
            "increment":"u,0,0",
            "count":8,
          },

          "Pillar":{
            "offset":"u*8,0,0",
            "increment":"0,0,u",
            "count":9,
          }
        },


       "":{
         "Door":{"offset":"0,u,u+1"}
       },


       "stairs":{
         "Stairs":{
           "offset":"0,0,u",
           "angle":"0,0,0",
         },

         "Stairs":{
           "offset":"u*3,u,u",
           "angle":"0,90,0",
         },

         "Block":{
           "offset":"u,0,u",
           "angle":"0,0,0",
         },

         "Block":{
           "offset":"u*2,0,u",
           "angle":"0,0,0",
         }
       }

      }
    }
  }
}
)";


  load(obj_s,stg_s);
#endif

  main_renderer.change_base_point(160,80);

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


	

