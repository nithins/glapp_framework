#ifndef __LIGHT_IMPL_H_INCLUDED
#define __LIGHT_IMPL_H_INCLUDED

#include <light.h>
#include <boost/shared_ptr.hpp>

class TLight;

#ifdef QT_GUI_LIB
#include <QFrame>

namespace Ui
{
  class Lightmanager_Frame;
}

#endif

class TLightManager:
#ifdef QT_GUI_LIB
      public QFrame,
#endif
      virtual public ILightManager
{
#ifdef QT_GUI_LIB
    Q_OBJECT
#endif

  public:
    TLightManager ( int numlights, IFramework *f );

    virtual ~TLightManager();

    virtual int Render() const ;

    virtual std::string Name() const
    {
      return "Light Manager";
    }

#ifdef QT_GUI_LIB
    virtual QFrame * getQFrame()
    {
      return this;
    }
#endif

  private:

    IFramework * m_framework;
    int          m_num_lights;
    std::vector<boost::shared_ptr<TLight> > m_lights;
    bool         m_dirty;

//    float        m_ambient[4];
    float        m_specular[4];
//    float        m_diffuse[4];
    float        m_emission[4];
    int          m_color_material_mode;
    int          m_shininess;


    void render_light_params()const ;



#ifdef QT_GUI_LIB
    Ui::Lightmanager_Frame * m_ui;

    int m_active_ui_light;

    void update_lights_ui();


  private Q_SLOTS:

//    void on_material_ambient_colorpicker_colorChanged(const QColor &);
//    void on_material_diffuse_colorpicker_colorChanged(const QColor &);

    void on_material_specular_colorpicker_colorChanged(const QColor &);
    void on_material_emission_colorpicker_colorChanged(const QColor &);

    void on_material_shininess_spinBox_valueChanged(int );
    void on_light_number_spinBox_valueChanged(int );

    void on_light_type_comboBox_currentIndexChanged(int);

    void on_light_ambient_colorpicker_colorChanged(const QColor &);
    void on_light_diffuse_colorpicker_colorChanged(const QColor &);
    void on_light_specular_colorpicker_colorChanged(const QColor &);

    void on_show_light_graphics_checkBox_clicked(const bool &);


#endif

};

#endif// __LIGHT_H_INCLUDED
