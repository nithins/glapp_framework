#include <algorithm>

#include <GL/glew.h>

#include <tri_stripper.h>
#include <glutils_impl.h>
#include <GLSLProgram.h>


// file generated by copy_files_to_c_header.py
// to include shader source code as c strings
#include <shadersources.h>

namespace glutils
{
  class buffered_triangles_ren_t: virtual public renderable_t
  {

  private:
    bufobj_ptr_t m_ver_bo;
    bufobj_ptr_t m_tri_bo;
    bufobj_ptr_t m_col_bo;
    bufobj_ptr_t m_nrm_bo;

    int ( buffered_triangles_ren_t::*render_func ) () const;

    int m_num_triangles;

    double m_extent[6];


  public:
    buffered_triangles_ren_t
        ( const bufobj_ptr_t & ver_buf,
          const bufobj_ptr_t & tri_buf,
          const bufobj_ptr_t & col_buf )
    {

      m_ver_bo   = ver_buf;
      m_tri_bo   = tri_buf;
      m_col_bo   = col_buf;

      if ( m_ver_bo->id() == 0 && m_ver_bo->src_ptr() == NULL )
      {
        _ERROR ( "no vertex data specified neither in cpu nor gpu" );
      }

      if ( m_tri_bo->id() == 0 && m_tri_bo->src_ptr() == NULL )
      {
        _ERROR ( "no triangle index data specified neither in cpu nor gpu" );
      }

      if ( m_col_bo->id() == 0 && m_col_bo->src_ptr() == NULL )
      {
        render_func = &buffered_triangles_ren_t::render_without_color;
      }
      else
      {
        render_func = &buffered_triangles_ren_t::render_with_color;
      }

      double * normals = compute_normals ( m_ver_bo, m_tri_bo );

      m_nrm_bo  = buf_obj_t::create_bo
                  ( normals, GL_DOUBLE, 3, GL_ARRAY_BUFFER,
                   sizeof ( double ) *3*m_ver_bo->get_num_items(), 0 );

      compute_extent ( m_ver_bo, m_extent );

      m_num_triangles = m_tri_bo->get_num_items();

      delete []normals;

    }

    virtual int render() const
    {
      return ( this->*render_func ) ();
    }

    int render_with_color() const
    {
      m_col_bo->bind_to_color_pointer();
      m_ver_bo->bind_to_vertex_pointer();
      m_nrm_bo->bind_to_normal_pointer();

      glBindBuffer ( m_tri_bo->target(), m_tri_bo->id() );
      glDrawElements ( GL_TRIANGLES, m_num_triangles*3, m_tri_bo->src_type(), 0 );
      glBindBuffer ( m_tri_bo->target(), 0 );

      m_nrm_bo->unbind_from_normal_pointer();
      m_col_bo->unbind_from_color_pointer();
      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_triangles*3;
    }

    int render_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();
      m_nrm_bo->bind_to_normal_pointer();

      glBindBuffer ( m_tri_bo->target(), m_tri_bo->id() );
      glDrawElements ( GL_TRIANGLES, m_num_triangles*3, m_tri_bo->src_type(), 0 );
      glBindBuffer ( m_tri_bo->target(), 0 );

      m_nrm_bo->unbind_from_normal_pointer();
      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_triangles*3;
    }


    virtual ~buffered_triangles_ren_t()
    {
    }

    virtual bool get_extent ( double * extent )
    {
      std::copy ( m_extent, m_extent + 6, extent );
      return true;
    }
  };

  class buffered_flat_triangles_ren_t: virtual public renderable_t
  {

  private:
    bufobj_ptr_t m_ver_bo;
    bufobj_ptr_t m_tri_bo;
    bufobj_ptr_t m_col_bo;

    int ( buffered_flat_triangles_ren_t::*render_func ) () const;

    int m_num_triangles;

    double m_extent[6];

    static GLSLProgram * s_flat_tri_shader;

  public:
    buffered_flat_triangles_ren_t
        ( const bufobj_ptr_t & ver_buf,
          const bufobj_ptr_t & tri_buf,
          const bufobj_ptr_t & col_buf )
    {

      m_ver_bo   = ver_buf;
      m_tri_bo   = tri_buf;
      m_col_bo   = col_buf;

      if ( s_flat_tri_shader == NULL )
      {
        s_flat_tri_shader = GLSLProgram::createFromSourceStrings
                            (
                                std::string ( flat_triangles_vert ),
                                std::string ( flat_triangles_geom ),
                                std::string (),
                                GL_TRIANGLES,
                                GL_TRIANGLES
                                );

        std::string flat_tri_shader_log;

        s_flat_tri_shader->GetProgramLog ( flat_tri_shader_log );

        _LOG_VAR ( flat_tri_shader_log );

      }

      if ( m_ver_bo->id() == 0 && m_ver_bo->src_ptr() == NULL )
      {
        _ERROR ( "no vertex data specified neither in cpu nor gpu" );
      }

      if ( m_tri_bo->id() == 0 && m_tri_bo->src_ptr() == NULL )
      {
        _ERROR ( "no triangle index data specified neither in cpu nor gpu" );
      }

      if ( m_col_bo->id() == 0 && m_col_bo->src_ptr() == NULL )
      {
        render_func = &buffered_flat_triangles_ren_t::render_without_color;
      }
      else
      {
        render_func = &buffered_flat_triangles_ren_t::render_with_color;
      }

      compute_extent ( m_ver_bo, m_extent );

      m_num_triangles = m_tri_bo->get_num_items();

    }

    virtual int render() const
    {
      return ( this->*render_func ) ();
    }

    int render_with_color() const
    {
      m_col_bo->bind_to_color_pointer();
      m_ver_bo->bind_to_vertex_pointer();

      s_flat_tri_shader->use();

      glBindBuffer ( m_tri_bo->target(), m_tri_bo->id() );
      glDrawElements ( GL_TRIANGLES, m_num_triangles*3, m_tri_bo->src_type(), 0 );
      glBindBuffer ( m_tri_bo->target(), 0 );

      s_flat_tri_shader->disable();

      m_col_bo->unbind_from_color_pointer();
      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_triangles*3;
    }

    int render_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      s_flat_tri_shader->use();

      glBindBuffer ( m_tri_bo->target(), m_tri_bo->id() );
      glDrawElements ( GL_TRIANGLES, m_num_triangles*3, m_tri_bo->src_type(), 0 );
      glBindBuffer ( m_tri_bo->target(), 0 );

      s_flat_tri_shader->disable();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_triangles*3;
    }


    virtual ~buffered_flat_triangles_ren_t()
    {
    }

    virtual bool get_extent ( double * extent )
    {
      std::copy ( m_extent, m_extent + 6, extent );
      return true;
    }
  };

  GLSLProgram * buffered_flat_triangles_ren_t::s_flat_tri_shader = NULL;

  class buffered_tristrip_ren_t: virtual public renderable_t
  {

  private:

    GLuint m_render_dl;

    double m_extent[6];

  public:
    buffered_tristrip_ren_t
        ( const bufobj_ptr_t & ver_buf,
          const bufobj_ptr_t & tri_buf,
          const bufobj_ptr_t & col_buf )
    {

      bufobj_ptr_t tri_bo = tri_buf;
      bufobj_ptr_t ver_bo = ver_buf;
      bufobj_ptr_t col_bo = col_buf;
      bufobj_ptr_t nrm_bo;

      if ( ver_bo->id() == 0 && ver_bo->src_ptr() == NULL )
      {
        _ERROR ( "no vertex data specified neither in cpu nor gpu" );
      }

      if ( tri_bo->id() == 0 && tri_bo->src_ptr() == NULL )
      {
        _ERROR ( "no triangle index data specified neither in cpu nor gpu" );
      }

      bool use_color = true;

      if ( col_bo->id() == 0 && col_bo->src_ptr() == NULL )
      {
        use_color = false;
      }

      double * normals = compute_normals ( ver_bo, tri_bo );

      nrm_bo = buf_obj_t::create_bo
          ( normals, GL_DOUBLE, 3, GL_ARRAY_BUFFER,
            sizeof ( double ) *3*ver_bo->get_num_items(), 0 );

      triangle_stripper::indices t_inds_vec;

      for ( unsigned int i = 0 ; i < tri_bo->get_num_items();i++ )
      {
        for ( uint j = 0 ; j < 3;j++ )
        {
          t_inds_vec.push_back
              ( glutils::gl_get_num_val<uint>
                ( tri_bo->get_item_comp_ptr ( i, j ), tri_bo->src_type() )
                );
        }
      }

      triangle_stripper::primitive_vector prims_vec;

      triangle_stripper::tri_stripper t_stripper ( t_inds_vec );

      t_stripper.SetMinStripSize ( 2 );
      t_stripper.SetCacheSize ( 10 );
      t_stripper.SetBackwardSearch ( false );

      t_stripper.Strip ( &prims_vec );

      m_render_dl = glGenLists ( 1 );

      ver_bo->bind_to_vertex_pointer();
      nrm_bo->bind_to_normal_pointer();

      if ( use_color )
        col_bo->bind_to_color_pointer();

      glNewList ( m_render_dl, GL_COMPILE );

      for ( size_t i = 0; i < prims_vec.size(); ++i )
      {
        GLuint type = ( prims_vec[i].Type == triangle_stripper::TRIANGLE_STRIP ) ? ( GL_TRIANGLE_STRIP ) : ( GL_TRIANGLES );

        glBegin ( type );

        for ( uint j = 0 ; j < prims_vec[i].Indices.size();++j )
        {
          glArrayElement ( prims_vec[i].Indices[j] );
        }

        glEnd();
      }

      glEndList();

      nrm_bo->unbind_from_normal_pointer();
      ver_bo->unbind_from_vertex_pointer();

      if ( use_color )
        col_bo->unbind_from_color_pointer();

      compute_extent ( ver_bo, m_extent );

      delete []normals;

    }

    virtual int render() const
    {
      glCallList ( m_render_dl );
      return 0;
    }


    virtual ~buffered_tristrip_ren_t()
    {

      if ( m_render_dl )
        glDeleteLists ( m_render_dl, 1 );

      m_render_dl = 0;
    }

    virtual bool get_extent ( double * extent )
    {
      std::copy ( m_extent, m_extent + 6, extent );
      return true;
    }
  };



  renderable_t * create_buffered_triangles_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t t,
        bufobj_ptr_t c )
  {
    return new buffered_triangles_ren_t ( v, t, c );
  }

  renderable_t * create_buffered_tristrip_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t t,
        bufobj_ptr_t c )
  {
    return new buffered_tristrip_ren_t ( v, t, c );
  }

  renderable_t * create_buffered_flat_triangles_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t t,
        bufobj_ptr_t c )
  {
    return new buffered_flat_triangles_ren_t ( v, t, c );
  }
}
