#ifndef ISOM_Dot_HPP_INCLUDED
#define ISOM_Dot_HPP_INCLUDED


#include"isom_image.hpp"
#include"isom_point.hpp"
#include"isom_transformer.hpp"
#include"isom_renderer.hpp"




struct
Dot
{
  Point  point;
  Color  color;

  constexpr Dot(int  x_, int  y_, int  z_, const Color&  color_):
  point(x_,y_,z_),
  color(color_){}

  void  transform(const Transformer&  tr);

  void  render(Renderer&  dst) const;

};




#endif




