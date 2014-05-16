/*
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
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
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <limits.h>
#include "VNSIChannelScan.h"
#include "responsepacket.h"
#include "requestpacket.h"
#include "vnsicommand.h"
#include "libXBMC_gui.h"

#include <sstream>


#define BUTTON_START                    5
#define BUTTON_BACK                     6
#define BUTTON_CANCEL                   7
#define HEADER_LABEL                    8

#define SPIN_CONTROL_SOURCE_TYPE        10
#define CONTROL_RADIO_BUTTON_TV         11
#define CONTROL_RADIO_BUTTON_RADIO      12
#define CONTROL_RADIO_BUTTON_FTA        13
#define CONTROL_RADIO_BUTTON_SCRAMBLED  14
#define CONTROL_RADIO_BUTTON_HD         15
#define CONTROL_SPIN_COUNTRIES          16
#define CONTROL_SPIN_SATELLITES         17
#define CONTROL_SPIN_DVBC_INVERSION     18
#define CONTROL_SPIN_DVBC_SYMBOLRATE    29
#define CONTROL_SPIN_DVBC_QAM           20
#define CONTROL_SPIN_DVBT_INVERSION     21
#define CONTROL_SPIN_ATSC_TYPE          22

#define LABEL_TYPE                      30
#define LABEL_DEVICE                    31
#define PROGRESS_DONE                   32
#define LABEL_TRANSPONDER               33
#define LABEL_SIGNAL                    34
#define PROGRESS_SIGNAL                 35
#define LABEL_STATUS                    36

#define CHANNEL_SCANNER_NAME            "XBMC channel scanner"

using namespace ADDON;

cVNSIChannelScan::cVNSIChannelScan()
 : m_iPort(0)
{
}

cVNSIChannelScan::~cVNSIChannelScan()
{
}

void* cVNSIChannelScan::Process()
{
  cResponsePacket*      vresp       = NULL;
  CAddonGUIProgressBar* progressBar = NULL;

  try
  {
    progressBar = GUI->ProgressBar_create(CHANNEL_SCANNER_NAME);
    if (!progressBar)
      throw false;

    if (!m_vnsiData.Open(m_strHostname, m_iPort, CHANNEL_SCANNER_NAME))
      throw false;

    if (!m_vnsiData.Login())
      throw false;

    cRequestPacket vrp;
    cResponsePacket* vresp = NULL;
    uint32_t retCode = VNSI_RET_ERROR;
    if (!vrp.init(VNSI_SCAN_START))
      throw false;

    vresp = m_vnsiData.ReadResult(&vrp);
    if (!vresp)
      throw false;

    retCode = vresp->extract_U32();
    if (retCode != VNSI_RET_OK)
      throw false;

    while (!IsStopped())
    {
      cRequestPacket vrp;
      if (!vrp.init(VNSI_SCAN_PROGRESS))
        throw false;

      vresp = m_vnsiData.ReadResult(&vrp);
      if (!vresp)
        throw false;

      uint32_t retCode = vresp->extract_U32();
      if (retCode != VNSI_RET_OK)
        throw false;

      const float percentage = vresp->extract_Double();
      progressBar->SetPercentage(percentage);

      const unsigned int frequencyHz = vresp->extract_U32();
      char text[16];
      snprintf(text, sizeof(text), "%d MHz", frequencyHz / (1000 * 1000));
      progressBar->SetText(text);

      delete vresp;
      vresp = NULL;

      Sleep(500);
    }
  }
  catch (bool bSuccess)
  {
  }

  if (progressBar)
    progressBar->MarkFinished();
  GUI->ProgressBar_destroy(progressBar);
  m_vnsiData.Close();
  delete vresp;
  return NULL;
}
