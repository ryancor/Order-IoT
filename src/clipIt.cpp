#include <iostream>
#include <string>
#include <wx/clipbrd.h>
#include <wx/event.h>
#include <wx/log.h>

using namespace std;

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

void OnChar(wxKeyEvent& event) {
  wxChar uc = event.GetUnicodeKey();
  if(uc != WXK_NONE) {
    if(uc >= 32) {
      wxLogMessage("You pressed '%c'\n", uc);
    } else {
      wxLogMessage("Nothing was pressed\n");
    }
  } else {// No Unicode equivalent.
    // It's a special key, deal with all the known ones:
    switch ( event.GetKeyCode() ) {
      case WXK_LEFT:
      case WXK_RIGHT:
          break;
      case WXK_F1:
          break;
      default:
          std::cout << uc;
    }
  }
}
