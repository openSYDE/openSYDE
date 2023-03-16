//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view PC item data element (implementation)

   System view PC item data element

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>

#include "stwtypes.hpp"
#include "C_OscViewPc.hpp"
#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::opensyde_core;

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
C_OscViewPc::C_OscViewPc(void) :
   mq_Connected(false),
   mu32_BusIndex(std::numeric_limits<uint32_t>::max())
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewPc::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->mq_Connected, sizeof(this->mq_Connected), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mu32_BusIndex, sizeof(this->mu32_BusIndex), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get connected flag

   \return
   Current connected flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscViewPc::GetConnected(void) const
{
   return this->mq_Connected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get bus index

   \return
   Current bus index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscViewPc::GetBusIndex(void) const
{
   return this->mu32_BusIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connected state of pc in view

   \param[in]  oq_Connected         Flag if pc is connected
   \param[in]  ou32_BusIndex        Bus index PC is connected to
   \param[in]  orq_ForceSimpleSet   Optional flag to indicate if this function is used as a simple set or a logic operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewPc::SetConnected(const bool oq_Connected, const uint32_t ou32_BusIndex, const bool & orq_ForceSimpleSet)
{
   this->mq_Connected = oq_Connected;
   if ((orq_ForceSimpleSet == true) || (this->mq_Connected == true))
   {
      this->mu32_BusIndex = ou32_BusIndex;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync view bus index to added bus index

   \param[in]  ou32_Index  Added bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscViewPc::OnSyncBusAdded(const uint32_t ou32_Index)
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
void C_OscViewPc::OnSyncBusDeleted(const uint32_t ou32_Index)
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
bool C_OscViewPc::CheckIndirectDisconnection(void) const
{
   bool q_Retval;

   if ((this->mq_Connected == false) && (this->mu32_BusIndex != std::numeric_limits<uint32_t>::max()))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}
