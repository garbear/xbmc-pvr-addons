#pragma once
/*
 *      Copyright (C) 2013 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "addons/include/xbmc_service_types.h"
#include "../library.xbmc.addon/libXBMC_addon.h"

#ifdef _WIN32
#define SERVICE_HELPER_DLL "\\library.xbmc.service\\libXBMC_service" ADDON_HELPER_EXT
#else
#define SERVICE_HELPER_DLL_NAME "libXBMC_service-" ADDON_HELPER_ARCH ADDON_HELPER_EXT
#define SERVICE_HELPER_DLL "/library.xbmc.service/" SERVICE_HELPER_DLL_NAME
#endif

class CHelper_libXBMC_service
{
public:
  CHelper_libXBMC_service(void)
  {
    m_libXBMC_service = NULL;
    m_Handle          = NULL;
  }

  ~CHelper_libXBMC_service(void)
  {
    if (m_libXBMC_service)
    {
      SERVICE_unregister_me(m_Handle, m_Callbacks);
      dlclose(m_libXBMC_service);
    }
  }

  /*!
   * @brief Resolve all callback methods
   * @param handle Pointer to the add-on
   * @return True when all methods were resolved, false otherwise.
   */
  bool RegisterMe(void* handle)
  {
    m_Handle = handle;

    std::string libBasePath;
    libBasePath  = ((cb_array*)m_Handle)->libPath;
    libBasePath += SERVICE_HELPER_DLL;

#if defined(ANDROID)
      struct stat st;
      if(stat(libBasePath.c_str(),&st) != 0)
      {
        std::string tempbin = getenv("XBMC_ANDROID_LIBS");
        libBasePath = tempbin + "/" + SERVICE_HELPER_DLL_NAME;
      }
#endif

    m_libXBMC_service = dlopen(libBasePath.c_str(), RTLD_LAZY);
    if (m_libXBMC_service == NULL)
    {
      fprintf(stderr, "Unable to load %s\n", dlerror());
      return false;
    }

    SERVICE_register_me = (void* (*)(void *HANDLE))
      dlsym(m_libXBMC_service, "SERVICE_register_me");
    if (SERVICE_register_me == NULL) { fprintf(stderr, "Unable to assign function %s\n", dlerror()); return false; }

    SERVICE_unregister_me = (void (*)(void* HANDLE, void* CB))
      dlsym(m_libXBMC_service, "SERVICE_unregister_me");
    if (SERVICE_unregister_me == NULL) { fprintf(stderr, "Unable to assign function %s\n", dlerror()); return false; }

    m_Callbacks = SERVICE_register_me(m_Handle);
    return m_Callbacks != NULL;
  }

protected:
  void* (*SERVICE_register_me)(void*);
  void (*SERVICE_unregister_me)(void*, void*);

private:
  void* m_libXBMC_service;
  void* m_Handle;
  void* m_Callbacks;
  struct cb_array
  {
    const char* libPath;
  };
};
