#include <iostream>
#include <string>

#ifdef __APPLE__
#include <wx/clipbrd.h>
#include <wx/event.h>
#include <wx/log.h>
#endif

#ifdef __unix__
#include <QClipboard>
#include <QApplication>
#endif

using namespace std;


#ifdef __APPLE__
void copyRecToClip(string clip) {
  // get text to copy to clip
  if(wxTheClipboard->Open()) {
    wxTheClipboard->SetData(new wxTextDataObject(clip));
    wxTheClipboard->Close();
  }

  // read it
  if(wxTheClipboard->Open()) {
    if(wxTheClipboard->IsSupported(wxDF_TEXT)) {
      wxTextDataObject data;
      wxTheClipboard->GetData(data);

      const char *new_text = data.GetText();
      //wxInfoMessageBox((wxWindow*)new_text);
      std::cout << "Copied to Clipboard." << std::endl;
    }
    wxTheClipboard->Close();
  }
}
#endif


#ifdef __unix__
void QtOnChar(QString clip) {
  #ifdef Q_WS_X11
    bool useGUI = true;
  #else
    bool useGUI = true;
  #endif

  int argc;
  char **argv;
  QApplication app(argc, argv, useGUI);

  if(useGUI) {
    QString selected_text;
    QString text = clip;

    selected_text.append(text);
    QApplication::clipboard()->setText(selected_text);

    std::cout << "Copied to Clipboard." << std::endl;

    qApp->exit();
  }
}
#endif
