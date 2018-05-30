#include <string>

#ifdef __APPLE__
  #include <wx/event.h>

  void copyRecToClip(std::string);
#endif

#ifdef __unix__
  #include <QApplication>

  void QtOnChar(QString);
#endif
