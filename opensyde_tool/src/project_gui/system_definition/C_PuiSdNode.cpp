//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI node: stores additional UI information (implementation)

   UI node: stores additional UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiSdNode.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Initialize default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeConnection::C_PuiSdNodeConnection()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNodeConnection::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;
   float64 f64_Value;

   this->c_ConnectionID.CalcHash(oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_UINodeConnectionInteractionPoints.size(); ++u32_Counter)
   {
      f64_Value = this->c_UINodeConnectionInteractionPoints[u32_Counter].x();
      stw_scl::C_SCLChecksums::CalcCRC32(&f64_Value, sizeof(f64_Value), oru32_HashValue);

      f64_Value = this->c_UINodeConnectionInteractionPoints[u32_Counter].y();
      stw_scl::C_SCLChecksums::CalcCRC32(&f64_Value, sizeof(f64_Value), oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Initialize default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNode::C_PuiSdNode() :
   C_PuiBsBox()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNode::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->c_UIBusConnections.size(); ++u32_Counter)
   {
      this->c_UIBusConnections[u32_Counter].CalcHash(oru32_HashValue);
   }

   for (u32_Counter = 0U; u32_Counter < this->c_UIDataPools.size(); ++u32_Counter)
   {
      this->c_UIDataPools[u32_Counter].CalcHash(oru32_HashValue);
   }

   for (u32_Counter = 0U; u32_Counter < this->c_UICanProtocols.size(); ++u32_Counter)
   {
      this->c_UICanProtocols[u32_Counter].CalcHash(oru32_HashValue);
   }

   C_PuiBsBox::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specified connection

   \param[in] orc_ConnectionID ID of connection to delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdNode::DeleteConnection(const C_PuiSdNodeConnectionId & orc_ConnectionID)
{
   for (uint32 u32_Index = 0; u32_Index < this->c_UIBusConnections.size(); ++u32_Index)
   {
      C_PuiSdNodeConnection & rc_BusConnection = this->c_UIBusConnections[u32_Index];
      if (rc_BusConnection.c_ConnectionID == orc_ConnectionID)
      {
         this->c_UIBusConnections.erase(this->c_UIBusConnections.begin() + u32_Index);
         break;
      }
   }
}
