#include <id.h>
#include <logutil.h>

class IID_Base::IID_Base_impl
{
  static unsigned int s_id;

  unsigned int m_id;

public:

  IID_Base_impl()
  {
    m_id = s_id++;
  }

  ~IID_Base_impl()
  {
  }

  inline unsigned int GetID()
  {
    return m_id;
  }
};

unsigned int IID_Base::IID_Base_impl::s_id = 0;

IID_Base::IID_Base()
{
  m_pImpl.reset(new IID_Base_impl);
}

IID_Base::~IID_Base()
{
}

IID::~IID()
{
}

unsigned int IID_Base::GetID() const
{
  return m_pImpl->GetID();
}

std::string IID::Name() const
{
  return "Unknown";
}


