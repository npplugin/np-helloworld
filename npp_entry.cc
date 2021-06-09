/* ***** BEGIN LICENSE BLOCK *****
* Version: MPL 1.1/GPL 2.0/LGPL 2.1
* This code was based on the npsimple.c sample code in Gecko-sdk.
*
* The contents of this file are subject to the Mozilla Public License Version
* 1.1 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* Contributor(s):
*
* Alternatively, the contents of this file may be used under the terms of
* either the GNU General Public License Version 2 or later (the "GPL"), or
* the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
* in which case the provisions of the GPL or the LGPL are applicable instead
* of those above. If you wish to allow use of your version of this file only
* under the terms of either the GPL or the LGPL, and not to allow others to
* use your version of this file under the terms of the NPL, indicate your
* decision by deleting the provisions above and replace them with the notice
* and other provisions required by the GPL or the LGPL. If you do not delete
* the provisions above, a recipient may use your version of this file under
* the terms of any one of the NPL, the GPL or the LGPL.
* ***** END LICENSE BLOCK ***** */

//#include <stdio.h>
#include "mylog.h"
#include "plugin.h"

NPError NPP_GetValue(NPP instance, NPPVariable variable, void* value) {
    MY_LOG("");
    switch (variable) {
    default:
        return NPERR_GENERIC_ERROR;
    case NPPVpluginNameString:
        *((char**)value) = "ScreenCapturePlugin";
        break;
    case NPPVpluginDescriptionString:
        *((char**)value) = "ScreenCapturePlugin plugin.";
        break;
    case NPPVpluginScriptableNPObject: {
        if (instance == NULL)
            return NPERR_INVALID_INSTANCE_ERROR;
        CPlugin* pPlugin = (CPlugin*)instance->pdata;
        if (pPlugin == NULL)
            return NPERR_GENERIC_ERROR;
        *(NPObject**)value = (NPObject*)pPlugin->GetScriptableObject();
    }
                                     break;
    case NPPVpluginNeedsXEmbed:
        *((char*)value) = 1;
        break;
    }
    return NPERR_NO_ERROR;
}

NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved) {
    MY_LOG("");
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

#ifdef _WINDOWS
    int bWindowed = 1;
#else
    int bWindowed = 0;
#endif
    npnfuncs->setvalue(instance, NPPVpluginWindowBool, (void*)bWindowed);

    CPlugin* pPlugin = new CPlugin(instance);
    if (pPlugin == NULL)
        return NPERR_OUT_OF_MEMORY_ERROR;

    instance->pdata = (void*)pPlugin;

    return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP instance, NPSavedData** save) {
    MY_LOG("");
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    CPlugin* pPlugin = (CPlugin*)instance->pdata;
    if (pPlugin != NULL) {
        pPlugin->shut();
        delete pPlugin;
    }
    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(NPP instance, NPWindow* window) {
    MY_LOG("");
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    if (window == NULL)
        return NPERR_GENERIC_ERROR;

    CPlugin* pPlugin = (CPlugin*)instance->pdata;
    if (pPlugin == NULL)
        return NPERR_GENERIC_ERROR;

    MY_LOG("window->window = %p, %d", window->window, window->type);

    // window just created
      if (!pPlugin->isInitialized() && (window->window != NULL)) {
        if (!pPlugin->init(window)) {
          delete pPlugin;
          pPlugin = NULL;
          return NPERR_MODULE_LOAD_FAILED_ERROR;
        }
      }

    return NPERR_NO_ERROR;
}

NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype) {
    MY_LOG("");
    return NPERR_GENERIC_ERROR;
}

NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason) {
    MY_LOG("");
    return NPERR_GENERIC_ERROR;
}

int16_t NPP_HandleEvent(NPP instance, void* e) {
  NPEvent* event = (NPEvent*)e;
  MY_LOG("event:0x%x, 0x%x, 0x%x", event->event, event->lParam, event->wParam);

  switch (event->event) {
    case WM_PAINT: {
      CPlugin* pPlugin = (CPlugin*)instance->pdata;
      RECT rc;
      HDC hdc = pPlugin->GetHDC(&rc);

      FillRect(hdc, &rc, (HBRUSH)(COLOR_GRAYTEXT + 1));
      RECT r = rc;
      r.left += 10;
      r.top += 10;
      r.right -= 10;
      r.bottom -= 10;
      FillRect(hdc, &r, (HBRUSH)(COLOR_WINDOW + 1));

      //FrameRect(hdc, &rc, (HBRUSH)(COLOR_WINDOWFRAME + 1));

      SetTextColor(hdc, RGB(255, 0, 0));

      DrawText(hdc, TEXT("I am the np plugin."), -1, &rc,
               DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } break;
    default:
      break;
  }

  return 0;
}
