//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       System view PC item data element (implementation)

   System view PC item data element

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include <QFileInfo>
#include "C_Uti.h"
#include "stwtypes.h"
#include "C_PuiSvPc.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     21.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvPc::C_PuiSvPc(void) :
   C_PuiBsBox(),
   mq_Connected(false),
   mu32_BusIndex(std::numeric_limits<uint32>::max()),
   mc_CANDllPath("STW_dlls\\stwpeak2\\stwpeak2.dll")
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue Hash value with init [in] value and result [out] value

   \created     21.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvPc::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->mc_CANDllPath.toStdString().c_str(),
                                      this->mc_CANDllPath.length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mq_Connected, sizeof(this->mq_Connected), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mu32_BusIndex, sizeof(this->mu32_BusIndex), oru32_HashValue);
   this->mc_ConnectionData.CalcHash(oru32_HashValue);

   C_PuiBsBox::CalcHash(oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get connected flag

   \return
   Current connected flag

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSvPc::GetConnected(void) const
{
   return this->mq_Connected;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get bus index

   \return
   Current bus index

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_types::uint32 C_PuiSvPc::GetBusIndex(void) const
{
   return this->mu32_BusIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get connection UI data

   \return
   Current connection UI data

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const C_PuiBsLineBase & C_PuiSvPc::GetConnectionData(void) const
{
   return this->mc_ConnectionData;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set connection UI data

   \param[in] orc_Value New connection UI data

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvPc::SetConnectionData(const C_PuiBsLineBase & orc_Value)
{
   this->mc_ConnectionData = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set connected state of pc in view

   \param[in] oq_Connected       Flag if pc is connected
   \param[in] ou32_BusIndex      Bus index PC is connected to
   \param[in] orq_ForceSimpleSet Optional flag to indicate if this function is used as a simple set or a logic operation

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvPc::SetConnected(const bool oq_Connected, const uint32 ou32_BusIndex, const bool & orq_ForceSimpleSet)
{
   this->mq_Connected = oq_Connected;
   if ((orq_ForceSimpleSet == true) || (this->mq_Connected == true))
   {
      this->mu32_BusIndex = ou32_BusIndex;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the CAN DLL path

   \return
   CAN DLL path

   \created     05.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_PuiSvPc::GetCANDll(void) const
{
   return this->mc_CANDllPath;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the absolute CAN DLL path

   \return
   Absolute CAN DLL path

   \created     07.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_PuiSvPc::GetCANDllAbsolute(void) const
{
   QString c_Retval;

   if (this->mc_CANDllPath.compare("") != 0)
   {
      const QFileInfo c_Info(this->mc_CANDllPath);

      if (c_Info.isRelative() == true)
      {
         c_Retval = C_Uti::h_GetExePath() + '\\' + this->mc_CANDllPath;
      }
      else
      {
         c_Retval = this->mc_CANDllPath;
      }
   }
   else
   {
      c_Retval = this->mc_CANDllPath;
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set path for the CAN DLL

   \param[in]     orc_DllPath   Path for the CAN DLL

   \created     05.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSvPc::SetCANDll(const QString & orc_DllPath)
{
   this->mc_CANDllPath = orc_DllPath;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set UI box

   \param[in] orc_Box New UI box

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvPc::SetBox(const C_PuiBsBox & orc_Box)
{
   this->c_UIPosition = orc_Box.c_UIPosition;
   this->f64_Height = orc_Box.f64_Height;
   this->f64_Width = orc_Box.f64_Width;
   this->f64_ZOrder = orc_Box.f64_ZOrder;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sync view bus index to added bus index

   \param[in] ou32_Index Added bus index

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvPc::OnSyncBusAdded(const uint32 ou32_Index)
{
   if (ou32_Index < this->mu32_BusIndex)
   {
      ++this->mu32_BusIndex;
   }
   else if (ou32_Index == this->mu32_BusIndex)
   {
      if (this->mq_Connected == false)
      {
         this->mq_Connected = true;
      }
      else
      {
         ++this->mu32_BusIndex;
      }
   }
   else
   {
      //No handling necessary
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sync view bus index to deleted bus index

   \param[in] ou32_Index Deleted bus index

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvPc::OnSyncBusDeleted(const uint32 ou32_Index)
{
   if (ou32_Index < this->mu32_BusIndex)
   {
      --this->mu32_BusIndex;
   }
   else if (ou32_Index == this->mu32_BusIndex)
   {
      this->mq_Connected = false;
   }
   else
   {
      //No handling necessary
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility function to check if disconnect was only triggered indirectly

   \return
   False All valid state
   True  Disconnect was not triggered properly

   \created     04.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSvPc::CheckIndirectDisconnection(void) const
{
   bool q_Retval;

   if ((this->mq_Connected == false) && (this->mu32_BusIndex != std::numeric_limits<uint32>::max()))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}
