/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
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
 *
 * ***** END LICENSE BLOCK ***** */

 //////////////////////////////////////////////////
 //
 // CPlugin class implementation
 //

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <windowsx.h>

#include "mylog.h"
#include "plugin.h"

const char* kSayHello = "sayHello";

static NPClass plugin_ref_obj = {
  NP_CLASS_STRUCT_VERSION,
  ScriptablePluginObject::Allocate,
  ScriptablePluginObject::Deallocate,
  NULL,
  ScriptablePluginObject::HasMethod,
  ScriptablePluginObject::Invoke,
  ScriptablePluginObject::InvokeDefault,
  ScriptablePluginObject::HasProperty,
  ScriptablePluginObject::GetProperty,
  NULL,
  NULL,
};

ScriptablePluginObject::ScriptablePluginObject(NPP instance)
    : npp(instance) {
}

NPObject* ScriptablePluginObject::Allocate(NPP instance, NPClass* npclass) {
    return (NPObject*)(new ScriptablePluginObject(instance));
}

void ScriptablePluginObject::Deallocate(NPObject* obj) {
    delete (ScriptablePluginObject*)obj;
}

bool ScriptablePluginObject::HasMethod(NPObject* obj, NPIdentifier methodName) {
    return true;
}

bool ScriptablePluginObject::InvokeDefault(NPObject* obj, const NPVariant* args,
    uint32_t argCount, NPVariant* result) {
    return true;
}

bool ScriptablePluginObject::Invoke(NPObject* obj, NPIdentifier methodName,
    const NPVariant* args, uint32_t argCount,
    NPVariant* result) {
    ScriptablePluginObject* thisObj = (ScriptablePluginObject*)obj;
    char* name = npnfuncs->utf8fromidentifier(methodName);
    bool ret_val = false;
    if (!name) {
        return ret_val;
    }

    MY_LOG(name);

    if (!strcmp(name, kSayHello)) {
        ret_val = true;
        //TODO
        const char* outString = "hello world!\ncall from my plugin";
        char* npOutString = (char*)npnfuncs->memalloc(strlen(outString) + 1);
        if (!npOutString)
            return false;
        strcpy(npOutString, outString);
        STRINGZ_TO_NPVARIANT(npOutString, *result);
    }
    else {
        // Exception handling. 
        npnfuncs->setexception(obj, "Unknown method");
    }
    npnfuncs->memfree(name);
    return ret_val;
}

bool ScriptablePluginObject::HasProperty(NPObject* obj, NPIdentifier propertyName) {
    return false;
}

bool ScriptablePluginObject::GetProperty(NPObject* obj, NPIdentifier propertyName,
    NPVariant* result) {
    return false;
}

static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
static WNDPROC lpOldProc = NULL;

//HWND FindBrowserHWND(HWND hWnd) {
//    DWORD curTid = GetWindowThreadProcessId(hWnd, NULL);
//    DWORD browserTid = curTid;
//    HWND hBrowser = hWnd;
//
//    while (browserTid == curTid) {
//        hBrowser = GetParent(hBrowser);
//        browserTid = GetWindowThreadProcessId(hBrowser, NULL);
//        if (!hBrowser) {
//            break;
//        }
//    }
//    hBrowser = GetParent(hBrowser);
//    return hBrowser;
//}
CPlugin::CPlugin(NPP pNPInstance) :
    m_pNPInstance(pNPInstance),
    m_bInitialized(false),
    m_pScriptableObject(NULL) {
    m_hWnd = NULL;
}

CPlugin::~CPlugin() {
    if (m_pScriptableObject)
        npnfuncs->releaseobject((NPObject*)m_pScriptableObject);

    m_hWnd = NULL;
    m_bInitialized = false;
}

NPBool CPlugin::init(NPWindow* pNPWindow) {
    if (pNPWindow == NULL)
        return false;

    m_hWnd = (HWND)pNPWindow->window;
    if (m_hWnd == NULL)
        return false;

    // subclass window so we can intercept window messages and do our drawing to it
    lpOldProc = SubclassWindow(m_hWnd, (WNDPROC)PluginWinProc);
    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "hello, win32",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
        10, 10, 200, 100,
        m_hWnd,
        NULL, (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE), NULL);

//    HWND hBrowser = FindBrowserHWND(m_hWnd);
//    AttachThreadInput(GetWindowThreadProcessId(m_hWnd, NULL), GetWindowThreadProcessId(hBrowser, NULL), TRUE);

    m_Window = pNPWindow;
    m_bInitialized = true;
    return true;
}

void CPlugin::shut()
{
    SubclassWindow(m_hWnd, lpOldProc);
    m_hWnd = NULL;
    m_bInitialized = false;
}

NPBool CPlugin::isInitialized() {
    return m_bInitialized;
}

ScriptablePluginObject* CPlugin::GetScriptableObject() {
    if (!m_pScriptableObject) {
        m_pScriptableObject = (ScriptablePluginObject*)npnfuncs->createobject(m_pNPInstance, &plugin_ref_obj);

        // Retain the object since we keep it in plugin code
        // so that it won't be freed by browser.
        npnfuncs->retainobject((NPObject*)m_pScriptableObject);
    }

    return m_pScriptableObject;
}

HWND CPlugin::GetHWnd() {
    return m_hWnd;
}

static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_PAINT:
    {
        // draw a frame and display the string
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
        FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));
        CPlugin* p = (CPlugin*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if (p) {
            char* s = "hello, np plugin win32";
            DrawText(hdc, s, strlen(s), &rc, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
        }

        EndPaint(hWnd, &ps);
    }
    break;
    default:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
