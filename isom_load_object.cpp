#include"isom_load.hpp"
#include"libjson/json.hpp"
#include"expree_parser.hpp"
#include"expree_scope.hpp"
#include"expree_MemorySpace.hpp"




namespace{




Vertex
read_vertex(const libjson::Object&  o, expree::Scope&  parent)
{
  using namespace libjson;

  Vertex  v;

    for(auto&  m: o)
    {
      int  n = (m.value == ValueKind::number)? m.value->number:
               (m.value == ValueKind::string)? load_integer(parent,m.value->string):0;

           if(m.name == "x"){v.x = n;}
      else if(m.name == "y"){v.y = n;}
      else if(m.name == "z"){v.z = n;}
      else if(m.name == "r"){v.r = n;}
      else if(m.name == "g"){v.g = n;}
      else if(m.name == "b"){v.b = n;}
      else if(m.name == "u"){v.r = n;}
      else if(m.name == "v"){v.g = n;}
    }


  return v;
}


Line
read_line(const libjson::Object&  o, expree::Scope&  parent)
{
  using namespace libjson;

  Vertex  vertexes[2];

    for(auto&  m: o)
    {
        if(m.value == ValueKind::object)
        {
               if(m.name == "a"){vertexes[0] = read_vertex(m.value->object,parent);}
          else if(m.name == "b"){vertexes[1] = read_vertex(m.value->object,parent);}
        }
    }


  auto&  a = vertexes[0];
  auto&  b = vertexes[1];

  return Line(Dot(a,Color(a.r,a.g,a.b,255)),
              Dot(b,Color(b.r,b.g,b.b,255)));
}


Polygon
read_polygon(const libjson::Object&  o, expree::Scope&  parent)
{
  using namespace libjson;

  Vertex  vertexes[3];

    for(auto&  m: o)
    {
        if(m.value == ValueKind::object)
        {
               if(m.name == "a"){vertexes[0] = read_vertex(m.value->object,parent);}
          else if(m.name == "b"){vertexes[1] = read_vertex(m.value->object,parent);}
          else if(m.name == "c"){vertexes[2] = read_vertex(m.value->object,parent);}
        }
    }


  return Polygon(0,vertexes[0],
                   vertexes[1],
                   vertexes[2]);
}


Tetragon
read_tetragon(const libjson::Object&  o, expree::Scope&  parent)
{
  using namespace libjson;

  Vertex  vertexes[4];

    for(auto&  m: o)
    {
        if(m.value == ValueKind::object)
        {
               if(m.name == "a"){vertexes[0] = read_vertex(m.value->object,parent);}
          else if(m.name == "b"){vertexes[1] = read_vertex(m.value->object,parent);}
          else if(m.name == "c"){vertexes[2] = read_vertex(m.value->object,parent);}
          else if(m.name == "d"){vertexes[3] = read_vertex(m.value->object,parent);}
        }
    }


  return Tetragon(0,vertexes[0],
                    vertexes[1],
                    vertexes[2],
                    vertexes[3]);
}


Object
read_element(const libjson::Object&  jso, expree::Scope&  parent)
{
  expree::Scope  scope(parent);

  Object  o;

  using namespace libjson;

    for(auto&  m: jso)
    {
        if((m.name == "expressions") && (m.value == ValueKind::array))
        {
          read_expressions(m.value->array,scope);
        }

      else if(m.name == "line"    ){o.push(read_line(    m.value->object,scope));}
      else if(m.name == "polygon" ){o.push(read_polygon( m.value->object,scope));}
      else if(m.name == "tetragon"){o.push(read_tetragon(m.value->object,scope));}
    }


  return std::move(o);
}




Object
read_object(const libjson::ObjectMember&  m, expree::Scope&  parent)
{
  expree::Scope  scope(parent);

  Object  o;

  o.change_name(m.name.data());

  using namespace libjson;

    for(auto&  mm: m.value->object)
    {
        if((mm.name == "expressions") && (mm.value == ValueKind::array))
        {
          read_expressions(mm.value->array,scope);
        }

      else
        if((mm.name == "elements") && (mm.value == ValueKind::object))
        {
            for(auto&  mmm: mm.value->object)
            {
                if(mmm.value == ValueKind::object)
                {
                  o.push(read_element(mmm.value->object,scope));
                }

              else
                if((mmm.name == "expressions") && (mmm.value == ValueKind::array))
                {
                  read_expressions(mmm.value->array,scope);
                }
            }
        }
    }


  return std::move(o);
}


}


void
read_expressions(const libjson::Array&  arr, expree::Scope&  scope)
{
    for(auto&  v: arr)
    {
        if(v == libjson::ValueKind::string)
        {
          load_integer(scope,v->string);
        }
    }
}


int
load_integer(expree::Scope&  scope, const std::string&  s)
{
    try
    {
      expree::Parser  p(s);

      auto  result = p.make_element().make_value(&scope,true);

      result = expree::Object::remove_reference(result);

      return result.to_integer().data;
    }


    catch(expree::Exception&  e)
    {
      printf("%s\n",e.what());
    }


  return 0;
}


Object
load_object(const std::string&  s)
{
  expree::MemorySpace  memsp;

  auto&  scope = memsp.get_global_scope();

  Object  container;

  using namespace libjson;

  FileBuffer  fbuf(std::string(s.data()));

  Value  v(fbuf);

    if(v == ValueKind::object)
    {
        for(auto&  m: v->object)
        {
            if((m.name == "expressions") && (m.value == ValueKind::array))
            {
              read_expressions(m.value->array,scope);
            }

          else
            if((m.name == "objects") && (m.value == ValueKind::object))
            {
                for(auto&  mm: m.value->object)
                {
                    if((mm.name == "expressions") && (mm.value == ValueKind::array))
                    {
                      read_expressions(mm.value->array,scope);
                    }

                  else
                    if(mm.value == ValueKind::object)
                    {
                      container.push(read_object(mm,scope));
                    }
                }
            }
        }
    }


  return std::move(container);
}




