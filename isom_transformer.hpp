#ifndef ISOM_Transformer_HPP_INCLUDED
#define ISOM_Transformer_HPP_INCLUDED


#include"isom_math.hpp"
#include"isom_point.hpp"
#include"isom_vector.hpp"




class
Transformer
{
  static constexpr int  translation_flag = 1;
  static constexpr int  scaling_flag     = 2;
  static constexpr int  rotation_flag    = 4;

  int  flags;

  Angle  angle;

  Point  offset;
  Point  center;

  double  x_rate;
  double  y_rate;
  double  z_rate;

  double  x_sin_value;
  double  x_cos_value;
  double  y_sin_value;
  double  y_cos_value;
  double  z_sin_value;
  double  z_cos_value;

public:
  Transformer();

  void  change_angle(int  x, int  y, int  z);
  void  change_angle(const Angle&  a){change_angle(a.x,a.y,a.z);}

  void  change_scaling_rate(double  x, double  y, double  z);

  void  change_center(int  x, int  y, int  z);
  void  change_offset(int  x, int  y, int  z);

  void  change_center(const Point&  pt){change_center(pt.x,pt.y,pt.z);}
  void  change_offset(const Point&  pt){change_offset(pt.x,pt.y,pt.z);}


  const Angle&  get_angle()  const{return  angle;}
  const Point&  get_offset() const{return offset;}
  const Point&  get_center() const{return center;}


  void  set_translation_flag(){flags |= translation_flag;}
  void  set_scaling_flag()    {flags |=     scaling_flag;}
  void  set_rotation_flag()   {flags |=    rotation_flag;}

  void  unset_translation_flag(){flags &= ~translation_flag;}
  void  unset_scaling_flag()    {flags &=     ~scaling_flag;}
  void  unset_rotation_flag()   {flags &=    ~rotation_flag;}

  bool  test_translation_flag() const{return flags&translation_flag;}
  bool  test_scaling_flag()     const{return flags&    scaling_flag;}
  bool  test_rotation_flag()    const{return flags&   rotation_flag;}

  Point   operator()(const Point&   target) const;
  Vector  operator()(const Vector&  target) const;

};




#endif




