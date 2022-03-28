//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage class to group element IDs with their CRC

   Storage class to group element IDs with their CRC

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvDbElementIdCRCGroup.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
C_PuiSvDbElementIdCRCGroup::C_PuiSvDbElementIdCRCGroup() :
   mu32_Crc(0U)
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
sint32 C_PuiSvDbElementIdCRCGroup::UpdateCRC(void)
{
   return this->m_CalcCRC(this->mu32_Crc);
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
sint32 C_PuiSvDbElementIdCRCGroup::CheckCRC() const
{
   sint32 s32_Retval = C_NO_ERR;
   uint32 u32_NewCRC;

   if (this->m_CalcCRC(u32_NewCRC) == C_NO_ERR)
   {
      if (u32_NewCRC != this->mu32_Crc)
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
uint32 C_PuiSvDbElementIdCRCGroup::GetCRC(void) const
{
   return this->mu32_Crc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CRC

   \param[in]  ou32_CRC    CRC
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbElementIdCRCGroup::SetCRC(const uint32 ou32_CRC)
{
   this->mu32_Crc = ou32_CRC;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get element id

   \return
   Element id
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbNodeDataPoolListElementId & C_PuiSvDbElementIdCRCGroup::GetElementId() const
{
   return this->mc_ElementId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set element id

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbElementIdCRCGroup::SetElementId(const C_PuiSvDbNodeDataPoolListElementId & orc_Value)
{
   this->mc_ElementId = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc CRC

   \param[out]  oru32_Result  Result

   \return
   STW error codes

   \retval   C_NO_ERR   Checksum calculated
   \retval   C_CONFIG   ID not found
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDbElementIdCRCGroup::m_CalcCRC(uint32 & oru32_Result) const
{
   sint32 s32_Retval = C_NO_ERR;

   oru32_Result = 0xFFFFFFFFU;

   if (this->mc_ElementId.GetIsValid())
   {
      const C_OSCNodeDataPoolListElement * const pc_OSCListElement =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mc_ElementId.u32_NodeIndex,
                                                                    this->mc_ElementId.u32_DataPoolIndex,
                                                                    this->mc_ElementId.u32_ListIndex,
                                                                    this->mc_ElementId.u32_ElementIndex);
      const C_PuiSdNodeDataPoolListElement * const pc_UIListElement =
         C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mc_ElementId.u32_NodeIndex,
                                                                   this->mc_ElementId.u32_DataPoolIndex,
                                                                   this->mc_ElementId.u32_ListIndex,
                                                                   this->mc_ElementId.u32_ElementIndex);

      if ((pc_OSCListElement != NULL) && (pc_UIListElement != NULL))
      {
         const uint32 u32_Val = pc_OSCListElement->GetArraySize();
         //Data element core
         if (this->mc_ElementId.GetUseArrayElementIndex())
         {
            pc_OSCListElement->CalcHashElement(oru32_Result, this->mc_ElementId.GetArrayElementIndexOrZero());
         }
         else
         {
            pc_OSCListElement->CalcHash(oru32_Result);
         }
         //Include array size
         stw_scl::C_SCLChecksums::CalcCRC32(&u32_Val, sizeof(u32_Val), oru32_Result);
         //Data element UI
         pc_UIListElement->CalcHash(oru32_Result);
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
      //Signal part
      if (this->mc_ElementId.GetType() == C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL)
      {
         C_OSCCanMessageIdentificationIndices c_MessageId;
         uint32 u32_SignalIndex;
         C_PuiSdHandler::h_GetInstance()->ConvertElementIndexToSignalIndex(this->mc_ElementId, c_MessageId,
                                                                           u32_SignalIndex);
         {
            const C_OSCCanSignal * const pc_Signal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(c_MessageId,
                                                                                                   u32_SignalIndex);

            if (pc_Signal != NULL)
            {
               pc_Signal->CalcHash(oru32_Result);
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
      }
   }
   else
   {
      //No error because these can be copied
   }

   return s32_Retval;
}
