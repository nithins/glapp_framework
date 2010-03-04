#ifndef __VERTEX_RENDERER_INCLUDED_
#define __VERTEX_RENDERER_INCLUDED_

#ifndef __GLUTILS_H_INCLUDING_VERTEXRENDERER_H_
#error "This is not meant to be included directly . include glutils.h instead"
#endif

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cfloat>

#include <logutil.h>
#include <vecutils.h>


template < typename id_t, typename coord_t, typename color_t = coord_t, typename compare_t = std::less<id_t> >

class ArrayRenderer
{

    typedef unsigned int uint;

    struct _vertex
    {
      double x, y, z;

      _vertex()
      {}

      _vertex ( double _x, double _y, double _z ) :
          x ( _x ), y ( _y ), z ( _z )
      {}
    };

    struct _triangle
    {
      uint v[3];

      _triangle()
      {}

      _triangle ( uint v1, uint v2, uint v3 )
      {
        v[0] = v1;
        v[1] = v2;
        v[2] = v3;
      }
    };

    typedef _vertex _color;


  private:

    struct _textitem
    {
      _vertex     pos;
      std::string text;

      _textitem ( std::string _text, coord_t x, coord_t y, coord_t z ) : pos ( x, y, z ), text ( _text ) {}
    };

    std::map<id_t, unsigned int, compare_t>  m_ExtToIntID_map;
    std::vector<_vertex>                     m_vertices_vector;
    std::vector<_triangle>                   m_triangles_vector;
    std::map<unsigned int, _color >          m_vertex_color_map;
    std::vector<_textitem>                   m_textitems_vector;


    double       *m_vertices;
    double       *m_normals;
    double       *m_colors;
    unsigned int *m_triangle_indices;
    double        m_extent[6];

    unsigned int  m_num_vertices;
    unsigned int  m_num_triangle_indices;

    bool          m_bUseCommonColor;
    bool          m_bUseColor;

    void clear_renderdata()
    {

      if ( m_vertices != NULL )
      {
        delete []m_vertices;
        m_vertices = NULL;
      }

      if ( m_normals != NULL )
      {
        delete []m_normals;
        m_normals = NULL;
      }

      if ( m_triangle_indices != NULL )
      {
        delete []m_triangle_indices;
        m_triangle_indices = NULL;
      }

      m_num_vertices = 0;

      m_num_triangle_indices = 0;

    }

    void clear_inputdata()
    {
      m_ExtToIntID_map.clear();
      m_vertices_vector.clear();
      m_triangles_vector.clear();
      m_vertex_color_map.clear();
    }

    void prepare_color_for_render()
    {
      if ( m_bUseCommonColor )
        m_colors = new double[3];
      else
        m_colors = new double[m_vertices_vector.size() *3];

      if ( m_vertex_color_map.size() > 0 )
      {
        m_bUseColor = true;

        if ( !m_bUseCommonColor )
        {
          for ( unsigned int i = 0 ; i < m_vertices_vector.size();i++ )
          {
            typename std::map<unsigned int, _color>::iterator it = m_vertex_color_map.lower_bound ( i );

            if ( it == m_vertex_color_map.end() )
              it = m_vertex_color_map.upper_bound ( i );


            SETVEC3 ( ( &m_colors[3*i] ), it->second.x, it->second.y, it->second.z );
          }
        }
        else
        {
          typename std::map<unsigned int, _color>::iterator it = m_vertex_color_map.begin();

          SETVEC3 ( ( &m_colors[0] ), it->second.x, it->second.y, it->second.z );
        }
      }
    }

    void prepare_vertices_for_render()
    {
      m_num_vertices  = m_vertices_vector.size()  * 3;
      m_vertices      = new double[m_vertices_vector.size()  *3] ;

      for ( uint i = 0 ; i < m_vertices_vector.size();i++ )
      {
        SETVEC3 ( ( &m_vertices[3*i] ),
                  m_vertices_vector[i].x,
                  m_vertices_vector[i].y,
                  m_vertices_vector[i].z );
      }
    }

    void prepare_triangles_for_render()
    {
      m_num_triangle_indices = m_triangles_vector.size() * 3;
      m_triangle_indices     = new uint[m_triangles_vector.size() *3];

      for ( uint i = 0 ; i < m_triangles_vector.size();i++ )
      {

        SETVEC3 ( ( &m_triangle_indices[i*3] ),
                  m_triangles_vector[i].v[0],
                  m_triangles_vector[i].v[1],
                  m_triangles_vector[i].v[2] );
      }
    }

    void prepare_normals_for_render()
    {
      m_normals          = new double[m_vertices_vector.size() *3] ;

      for ( unsigned int i = 0 ; i < m_vertices_vector.size();i++ )
      {
        SETVEC3 ( ( &m_normals [3*i] ), 0.0, 0.0, 0.0 );
      }

      double n[3];

      for ( unsigned int i = 0 ; i < m_triangles_vector.size();i++ )
      {
        NORMALPTPTPT3 ( double, n,
                        ( &m_vertices[3*m_triangles_vector[i].v[0]] ),
                        ( &m_vertices[3*m_triangles_vector[i].v[1]] ),
                        ( &m_vertices[3*m_triangles_vector[i].v[2]] ) );

        ADDVECVEC3_ ( ( &m_normals[3*m_triangles_vector[i].v[0]] ), n );
        ADDVECVEC3_ ( ( &m_normals[3*m_triangles_vector[i].v[1]] ), n );
        ADDVECVEC3_ ( ( &m_normals[3*m_triangles_vector[i].v[2]] ), n );
      }

      for ( unsigned int i = 0 ; i < m_vertices_vector.size();i++ )
      {
        NORMALIZEVEC3_ ( double, ( &m_normals[i*3] ) );
      }
    }

    void prepare_extent()
    {
      double extent[] =
      {
        DBL_MAX, DBL_MIN,
        DBL_MAX, DBL_MIN,
        DBL_MAX, DBL_MIN
      };

      for ( uint i = 0 ;i < m_vertices_vector.size();i++ )
      {
        extent[0] = std::min ( m_vertices[i*3+0], extent[0] );
        extent[1] = std::max ( m_vertices[i*3+0], extent[1] );
        extent[2] = std::min ( m_vertices[i*3+1], extent[2] );
        extent[3] = std::max ( m_vertices[i*3+1], extent[3] );
        extent[4] = std::min ( m_vertices[i*3+2], extent[4] );
        extent[5] = std::max ( m_vertices[i*3+2], extent[5] );
      }

      std::copy ( extent, extent + 6, m_extent );
    }


  public:

    void clear()
    {
      m_textitems_vector.clear();

      clear_inputdata();
      clear_renderdata();
    }


    ArrayRenderer() :
        m_vertices ( NULL ),
        m_normals ( NULL ),
        m_triangle_indices ( NULL ),
        m_num_vertices ( 0 ),
        m_num_triangle_indices ( 0 ),
        m_bUseCommonColor ( false ),
        m_bUseColor ( false )
    {
    }

    ArrayRenderer ( const compare_t &comp ) :
        m_ExtToIntID_map ( comp ),
        m_vertices ( NULL ),
        m_normals ( NULL ),
        m_triangle_indices ( NULL ),
        m_num_vertices ( 0 ),
        m_num_triangle_indices ( 0 ),
        m_bUseCommonColor ( false ),
        m_bUseColor ( false )

    {
    }

    ~ArrayRenderer()
    {
      clear();
    }

    void add_vertex ( const id_t &id, const coord_t *v )
    {
      add_vertex ( id, v[0], v[1], v[2] );
    }

    void add_vertex ( const id_t &id, const coord_t &v1, const coord_t &v2, const coord_t &v3 )
    {
      if ( m_ExtToIntID_map.insert ( std::pair<id_t, unsigned int> ( id, m_vertices_vector.size() ) ).second == true )
        m_vertices_vector.push_back ( _vertex ( ( double ) v1, ( double ) v2, ( double ) v3 ) );
    }

    void set_common_color ( const color_t *c )
    {
      set_common_color ( c[0], c[1], c[2] );
    }

    void set_common_color ( const color_t &r, const color_t &g, const color_t &b )
    {
      m_bUseCommonColor     = true;
      m_vertex_color_map[0] = _color ( r, g, b );
    }

    void set_vertex_color ( const id_t &id, const color_t *c )
    {
      set_vertex_color ( id, c[0], c[1], c[2] );
    }

    void set_vertex_color ( const id_t &id, const color_t &r, const color_t &g, const color_t &b )
    {
      typename std::map<id_t, unsigned int, compare_t>::iterator it = m_ExtToIntID_map.find ( id );

      m_bUseCommonColor = false;

      if ( it != m_ExtToIntID_map.end() )
        m_vertex_color_map[it->second] = _color ( r, g, b );
      else
        _ERROR ( "did not find vertex with id = " << id );

    }

    void add_triangle (  const id_t *v )
    {
      add_triangle ( v[0], v[1], v[2] );
    }


    void add_triangle ( const id_t &id1, const id_t &id2, const id_t &id3 )
    {
      m_triangles_vector.push_back ( _triangle (
                                       m_ExtToIntID_map[id1],
                                       m_ExtToIntID_map[id2],
                                       m_ExtToIntID_map[id3] ) );
    }

    void add_quad ( const id_t &id1, const id_t &id2, const id_t &id3, const id_t &id4 )
    {
      m_triangles_vector.push_back ( _triangle (
                                       m_ExtToIntID_map[id1],
                                       m_ExtToIntID_map[id2],
                                       m_ExtToIntID_map[id4] ) );

      m_triangles_vector.push_back ( _triangle (
                                       m_ExtToIntID_map[id2],
                                       m_ExtToIntID_map[id3],
                                       m_ExtToIntID_map[id4] ) );
    }

    void add_arrow ( const id_t &id1, const id_t&id2 )
    {
      double p1[3];
      double p2[3];

      SETVEC3 ( p1,
                m_vertices_vector[m_ExtToIntID_map[id1]].x,
                m_vertices_vector[m_ExtToIntID_map[id1]].y,
                m_vertices_vector[m_ExtToIntID_map[id1]].z );

      SETVEC3 ( p2,
                m_vertices_vector[m_ExtToIntID_map[id2]].x,
                m_vertices_vector[m_ExtToIntID_map[id2]].y,
                m_vertices_vector[m_ExtToIntID_map[id2]].z );

      const double stem_size = 0.04;
      const double tip_length    = 0.5;
      const double tip_width     = 0.2;

      double u[3] = {0, 1, 0};
      double f[3];
      double r[3];

      double pts[7][3];

      SUBVECVEC3 ( f, p1, p2 );
      CROSSPROD3 ( r, u, f );
      CROSSPROD3 ( u, f, r );
      double mag = MAGVEC3 ( f );
      NORMALIZEVEC3_ ( double, r );
      NORMALIZEVEC3_ ( double, f );
      NORMALIZEVEC3_ ( double, u );

      COPYVEC3 ( pts[0], p1 );
      COPYVEC3 ( pts[1], p1 );

      COPYVEC3 ( pts[2], p2 );
      COPYVEC3 ( pts[3], p2 );

      COPYVEC3 ( pts[4], p2 );
      COPYVEC3 ( pts[5], p2 );

      COPYVEC3 ( pts[6], p2 );

      ADDVECVEC3_ ( pts[0], -stem_size*mag*r );
      ADDVECVEC3_ ( pts[1], stem_size*mag*r );
      ADDVECVEC3_ ( pts[2], -stem_size*mag*r );
      ADDVECVEC3_ ( pts[3], stem_size*mag*r );
      ADDVECVEC3_ ( pts[2], tip_length*mag*f );
      ADDVECVEC3_ ( pts[3], tip_length*mag*f );
      ADDVECVEC3_ ( pts[4], -tip_width*mag*r );
      ADDVECVEC3_ ( pts[5], tip_width*mag*r );
      ADDVECVEC3_ ( pts[4], tip_length*mag*f );
      ADDVECVEC3_ ( pts[5], tip_length*mag*f );

      unsigned int new_vert_start_index = m_vertices_vector.size();

      m_vertices_vector.push_back ( _vertex ( pts[0][0], pts[0][1], pts[0][2] ) );
      m_vertices_vector.push_back ( _vertex ( pts[1][0], pts[1][1], pts[1][2] ) );
      m_vertices_vector.push_back ( _vertex ( pts[2][0], pts[2][1], pts[2][2] ) );
      m_vertices_vector.push_back ( _vertex ( pts[3][0], pts[3][1], pts[3][2] ) );
      m_vertices_vector.push_back ( _vertex ( pts[4][0], pts[4][1], pts[4][2] ) );
      m_vertices_vector.push_back ( _vertex ( pts[5][0], pts[5][1], pts[5][2] ) );

      m_triangles_vector.push_back ( _triangle (
                                       new_vert_start_index + 1,
                                       new_vert_start_index + 2,
                                       new_vert_start_index + 0 ) );

      m_triangles_vector.push_back ( _triangle (
                                       new_vert_start_index + 2,
                                       new_vert_start_index + 1,
                                       new_vert_start_index + 3 ) );

      m_triangles_vector.push_back ( _triangle (
                                       new_vert_start_index + 4,
                                       new_vert_start_index + 5,
                                       m_ExtToIntID_map[id2] ) );

    }

    void add_text ( std::string str, coord_t x, coord_t y , coord_t z )
    {
      m_textitems_vector.push_back ( _textitem ( str, x, y, z ) );
    }

    void prepare_for_render()
    {
      clear_renderdata();

      prepare_color_for_render();

      prepare_vertices_for_render();

      prepare_triangles_for_render();

      prepare_normals_for_render();

      prepare_extent();
    }

    void get_extent ( coord_t *extent ) const
    {
      std::copy ( m_extent, m_extent + 6, extent );
    }

    void render_points() const
    {
      glEnableClientState ( GL_VERTEX_ARRAY );
      glVertexPointer ( 3, GL_DOUBLE, 0, m_vertices );

      if ( m_bUseColor && !m_bUseCommonColor )
        glEnableClientState ( GL_COLOR_ARRAY );

      if ( m_bUseColor && !m_bUseCommonColor )
        glColorPointer ( 3, GL_DOUBLE, 0, m_colors );

      if ( m_bUseColor && m_bUseCommonColor )
        glColor3dv ( m_colors );

      glDrawArrays ( GL_POINTS, 0, m_num_vertices / 3 );

      if ( m_bUseColor && !m_bUseCommonColor )
        glDisableClientState ( GL_COLOR_ARRAY );

      glDisableClientState ( GL_VERTEX_ARRAY );

    }

    void render_quads() const
    {
      render_triangles();
    }

    void render_triangles() const
    {
      glEnableClientState ( GL_VERTEX_ARRAY );
      glVertexPointer ( 3, GL_DOUBLE, 0, m_vertices );

      glEnableClientState ( GL_NORMAL_ARRAY );
      glNormalPointer ( GL_DOUBLE, 0, m_normals );

      if ( m_bUseColor && !m_bUseCommonColor )
        glEnableClientState ( GL_COLOR_ARRAY );

      if ( m_bUseColor && !m_bUseCommonColor )
        glColorPointer ( 3, GL_DOUBLE, 0, m_colors );

      if ( m_bUseColor && m_bUseCommonColor )
        glColor3dv ( m_colors );

      glDrawElements ( GL_TRIANGLES, m_num_triangle_indices, GL_UNSIGNED_INT, m_triangle_indices );

      if ( m_bUseColor && !m_bUseCommonColor )
        glDisableClientState ( GL_COLOR_ARRAY );

      glDisableClientState ( GL_VERTEX_ARRAY );

      glDisableClientState ( GL_NORMAL_ARRAY );
    }

    void render_text() const
    {
//       for ( uint i = 0 ; i < m_textitems_vector.size() ; i++ )
//       {
//         _textitem ti = m_textitems_vector[i];
//
//         glRasterPos3f ( ti.pos.x, ti.pos.y, ti.pos.z );
//
//         for ( uint j = 0 ; j < ti.text.size();j++ )
//         {
//           char c = ti.text[j];
//           glutBitmapCharacter ( GLUT_BITMAP_HELVETICA_12  , c );
//         }
//       }
    }

    /**
     * Call this after entering all data and preparing for rendering.. in case u want to save some memory
     */
    void sqeeze()
    {
      clear_inputdata();
    }
};

#endif

