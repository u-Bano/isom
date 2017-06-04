#ifndef ISOM_RENDERER_HPP_INCLUDED
#define ISOM_RENDERER_HPP_INCLUDED


#include<cstdint>
#include<vector>
#include"isom_point.hpp"
#include"isom_image.hpp"
#include"isom_DotSet.hpp"
#include"isom_LineContext.hpp"
#include"isom_FaceRenderingContext.hpp"
#include"isom_TextureRenderingContext.hpp"




struct Plane;


struct
Cell: public Color
{
  int32_t  z;

  const Plane*  plane;

};


class
Renderer
{
  std::vector<Cell>  table;

  int  width;
  int  height;

public:
  int  x_offset=0;
  int  y_offset=0;

  Renderer(int  w=0, int  h=0);

  void  clear();

  void  resize(int  w, int  h);

  int   get_width() const;
  int   get_height() const;

  bool  test(int  x, int  y) const
  {
    return((x >=     0) &&
           (y >=     0) &&
           (x <  width) &&
           (y < height));
  }

  void   set_cell(const Cell&  src, int  x, int  y);

        Cell&  get_cell(int  x, int  y)      ;
  const Cell&  get_cell(int  x, int  y) const;

  void  put(const Color&  color, int  x, int  y, int  z);
  void  put(const LineContext&  lc, const Color&  color);

  void  render_line(const Point&  p0, const Point&  p1, const Color&  color);
  void  render_dotset(const DotSet&  dotset);
  void  render_face(FaceRenderingContext&  ctx);
  void  render_texture(TextureRenderingContext&  ctx);

};




#endif




