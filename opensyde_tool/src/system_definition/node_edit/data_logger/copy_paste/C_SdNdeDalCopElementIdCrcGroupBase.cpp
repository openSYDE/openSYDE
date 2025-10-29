//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for groups of element IDs with their CRC

   Base class for groups of element IDs with their CRC

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SclChecksums.hpp"
#include "C_SdNdeDalCopElementIdCrcGroupBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalCopElementIdCrcGroupBase::C_SdNdeDalCopElementIdCrcGroupBase() :
   mu32_Crc(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalCopElementIdCrcGroupBase::~C_SdNdeDalCopElementIdCrcGroupBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update CRC

   \return
   STW error codes

   \retval   C_NO_ERR   Checksum calculated
   \retval   C_CONFIG   ID not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopElementIdCrcGroupBase::UpdateCrc(void)
{
   return this->m_CalcCrc(this->mu32_Crc);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check CRC

   \return
   STW error codes

   \retval   C_NO_ERR     Checksum does match
   \retval   C_CHECKSUM   Checksum doesnt match
   \retval   C_CONFIG     ID not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopElementIdCrcGroupBase::CheckCrc() const
{
   int32_t s32_Retval = C_NO_ERR;
   uint32_t u32_NewCrc;

   if (this->m_CalcCrc(u32_NewCrc) == C_NO_ERR)
   {
      if (u32_NewCrc != this->mu32_Crc)
      {
         s32_Retval = C_CHECKSUM;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CRC

   \return
   CRC
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDalCopElementIdCrcGroupBase::GetCrc(void) const
{
   return this->mu32_Crc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CRC

   \param[in]  ou32_Crc    CRC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalCopElementIdCrcGroupBase::SetCrc(const uint32_t ou32_Crc)
{
   this->mu32_Crc = ou32_Crc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc crc data pool element

   \param[out]  oru32_Result  Result
   \param[in]   orc_Id        Id

   \return
   STW error codes

   \retval   C_NO_ERR   Checksum calculated
   \retval   C_CONFIG   ID not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDalCopElementIdCrcGroupBase::mh_CalcCrcDataPoolElement(uint32_t & oru32_Result,
                                                                      const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNodeDataPoolListElement * const pc_OscListElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_Id.u32_NodeIndex,
                                                                 orc_Id.u32_DataPoolIndex,
                                                                 orc_Id.u32_ListIndex,
                                                                 orc_Id.u32_ElementIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UiListElement =
      C_PuiSdHandler::h_GetInstance()->GetUiDataPoolListElement(orc_Id.u32_NodeIndex,
                                                                orc_Id.u32_DataPoolIndex,
                                                                orc_Id.u32_ListIndex,
                                                                orc_Id.u32_ElementIndex);

   oru32_Result = 0xFFFFFFFFU;

   if ((pc_OscListElement != NULL) && (pc_UiListElement != NULL))
   {
      const uint32_t u32_Val = pc_OscListElement->GetArraySize();
      //Data element core
      if (orc_Id.GetUseArrayElementIndex())
      {
         pc_OscListElement->CalcHashElement(oru32_Result, orc_Id.GetArrayElementIndexOrZero());
      }
      else
      {
         pc_OscListElement->CalcHash(oru32_Result);
      }
      //Include array size
      stw::scl::C_SclChecksums::CalcCRC32(&u32_Val, sizeof(u32_Val), oru32_Result);
      //Data element UI
      pc_UiListElement->CalcHash(oru32_Result);
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}
