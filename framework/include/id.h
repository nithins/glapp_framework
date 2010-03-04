#ifndef __ID_HPP_DEFINED
#define __ID_HPP_DEFINED

#include <string>
#include <boost/shared_ptr.hpp>

class IID_Base
{

private:
  class IID_Base_impl;

  boost::shared_ptr<IID_Base_impl> m_pImpl;

public:

  unsigned int GetID() const;
  IID_Base();
  virtual ~IID_Base();

};


class IID: virtual public IID_Base
{

public:
  virtual std::string Name() const;

  virtual ~IID();
};

#endif
