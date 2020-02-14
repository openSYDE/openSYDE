//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view PC item data element (implementation)

   System view PC item data element

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <QFileInfo>

#include "C_Uti.h"
#include "C_PuiUtil.h"
#include "stwtypes.h"
#include "C_PuiSvPc.h"
#include "CSCLChecksums.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvPc::C_PuiSvPc(void) :
   C_PuiBsBox(),
   mq_Connected(false),
   mu32_BusIndex(std::numeric_limits<uint32>::max()),
   me_CANDllType(ePEAK),
   mc_CustomCANDllPath("")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvPc::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->me_CANDllType, sizeof(this->me_CANDllType), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->mc_CustomCANDllPath.toStdString().c_str(),
                                      this->mc_CustomCANDllPath.length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mq_Connected, sizeof(this->mq_Connected), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mu32_BusIndex, sizeof(this->mu32_BusIndex), oru32_HashValue);

   this->mc_ConnectionData.CalcHash(oru32_HashValue);

   C_PuiBsBox::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get connected flag

   \return
   Current connected flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvPc::GetConnected(void) const
{
   return this->mq_Connected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get bus index

   \return
   Current bus index
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_PuiSvPc::GetBusIndex(void) const
{
   return this->mu32_BusIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get connection UI data

   \return
   Current connection UI data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiBsLineBase & C_PuiSvPc::GetConnectionData(void) const
{
   return this->mc_ConnectionData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connection UI data

   \param[in]  orc_Value   New connection UI data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvPc::SetConnectionData(const C_PuiBsLineBase & orc_Value)
{
   this->mc_ConnectionData = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connected state of pc in view

   \param[in]  oq_Connected         Flag if pc is connected
   \param[in]  ou32_BusIndex        Bus index PC is connected to
   \param[in]  orq_ForceSimpleSet   Optional flag to indicate if this function is used as a simple set or a logic operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvPc::SetConnected(const bool oq_Connected, const uint32 ou32_BusIndex, const bool & orq_ForceSimpleSet)
{
   this->mq_Connected = oq_Connected;
   if ((orq_ForceSimpleSet == true) || (this->mq_Connected == true))
   {
      this->mu32_BusIndex = ou32_BusIndex;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN Dll type. (PEAK = 0,Vector = 1, Other = 2)

   \return   CAN Dll type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvPc::E_CANDllType C_PuiSvPc::GetCANDllType() const
{
   return this->me_CANDllType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the CAN DLL path

   \return
   CAN DLL path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvPc::GetCANDll(void) const
{
   QString c_Return;

   switch (this->me_CANDllType)
   {
   case ePEAK:
      c_Return = stw_opensyde_gui::mc_DLL_PATH_PEAK;
      break;
   case eVECTOR:
      c_Return = stw_opensyde_gui::mc_DLL_PATH_VECTOR;
      break;
   case eOTHER:
      c_Return = this->mc_CustomCANDllPath;
      break;
   default:
      c_Return = stw_opensyde_gui::mc_DLL_PATH_PEAK;
      break;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get custom CAN DLL Path

   \return Custom CAN DLL Path string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvPc::GetCustomCANDllPath() const
{
   return this->mc_CustomCANDllPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the absolute CAN DLL path or empty string if the path is empty

   \return
   Absolute CAN DLL path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvPc::GetCANDllAbsolute(void) const
{
   QString c_Return = this->GetCANDll();

   if (this->GetCANDll().isEmpty() == false)
   {
      // resolve variables and make absolute if it is relative
      c_Return = C_PuiUtil::h_GetResolvedAbsPathFromExe(this->GetCANDll());
   }
   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set DLL type. See get for type definition.

   \param[in]  oe_Type  CAN DLL type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvPc::SetCANDllType(const C_PuiSvPc::E_CANDllType oe_Type)
{
   this->me_CANDllType = oe_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set path for the CAN DLL

   \param[in]  orc_Path    Path for the CAN DLL
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvPc::SetCustomCANDllPath(const QString & orc_Path)
{
   this->mc_CustomCANDllPath = orc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set UI box

   \param[in]  orc_Box  New UI box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvPc::SetBox(const C_PuiBsBox & orc_Box)
{
   this->c_UIPosition = orc_Box.c_UIPosition;
   this->f64_Height = orc_Box.f64_Height;
   this->f64_Width = orc_Box.f64_Width;
   this->f64_ZOrder = orc_Box.f64_ZOrder;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to added bus index

   \param[in]  ou32_Index  Added bus index
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to deleted bus index

   \param[in]  ou32_Index  Deleted bus index
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to check if disconnect was only triggered indirectly

   \return
   False All valid state
   True  Disconnect was not triggered properly
*/
//----------------------------------------------------------------------------------------------------------------------
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
