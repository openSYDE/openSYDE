//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI class for dashboard parameterization table data (implementation)

   UI class for dashboard parameterization table data

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvDbParam.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
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

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbParam::C_PuiSvDbParam(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbParam::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->c_DataSetSelectionIndices.size(); ++u32_Counter)
   {
      const sint32 & rs32_Value = this->c_DataSetSelectionIndices[u32_Counter];
      stw_scl::C_SCLChecksums::CalcCRC32(&rs32_Value, sizeof(rs32_Value), oru32_HashValue);
   }

   for (u32_Counter = 0U; u32_Counter < this->c_ListValues.size(); ++u32_Counter)
   {
      this->c_ListValues[u32_Counter].CalcHash(oru32_HashValue);
   }

   for (u32_Counter = 0U; u32_Counter < this->c_ColWidth.size(); ++u32_Counter)
   {
      const std::vector<stw_types::sint32> & rc_ColWidths = this->c_ColWidth[u32_Counter];
      for (uint32 u32_Col = 0U; u32_Col < rc_ColWidths.size(); ++u32_Col)
      {
         const sint32 & rs32_Value = rc_ColWidths[u32_Col];
         stw_scl::C_SCLChecksums::CalcCRC32(&rs32_Value, sizeof(rs32_Value), oru32_HashValue);
      }
   }
   for (u32_Counter = 0U; u32_Counter < this->c_ExpandedItems.size(); ++u32_Counter)
   {
      const C_PuiSvDbExpandedTreeIndex & rc_CurItem = this->c_ExpandedItems[u32_Counter];
      rc_CurItem.c_ExpandedId.CalcHash(oru32_HashValue);
      stw_scl::C_SCLChecksums::CalcCRC32(&rc_CurItem.u32_Layer, sizeof(rc_CurItem.u32_Layer), oru32_HashValue);
   }
   for (u32_Counter = 0U; u32_Counter < this->c_ColPosIndices.size(); ++u32_Counter)
   {
      const sint32 & rs32_Value = this->c_ColPosIndices[u32_Counter];
      stw_scl::C_SCLChecksums::CalcCRC32(&rs32_Value, sizeof(rs32_Value), oru32_HashValue);
   }

   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if item is a read element

   \return
   True  Read element
   False Write element

   \created     25.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSvDbParam::IsReadElement(void) const
{
   return true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if param widget error

   \param[in]  orc_ListId List ID to check
   \param[out] orq_Error  Error output

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     20.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvDbParam::CheckError(const C_OSCNodeDataPoolListId & orc_ListId, bool & orq_Error) const
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(this->c_DataPoolElementsConfig.size() == this->c_ListValues.size());
   if (this->c_DataPoolElementsConfig.size() == this->c_ListValues.size())
   {
      tgl_assert(this->c_DataPoolElementsConfig.size() == this->c_DataSetSelectionIndices.size());
      if (this->c_DataPoolElementsConfig.size() == this->c_DataSetSelectionIndices.size())
      {
         const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            orc_ListId.u32_NodeIndex, orc_ListId.u32_DataPoolIndex, orc_ListId.u32_ListIndex);
         if (pc_List != NULL)
         {
            orq_Error = false;
            for (uint32 u32_ItConfig = 0;
                 ((u32_ItConfig < this->c_DataPoolElementsConfig.size()) && (orq_Error == false)) &&
                 (s32_Retval == C_NO_ERR);
                 ++u32_ItConfig)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_Config = this->c_DataPoolElementsConfig[u32_ItConfig];
               const C_OSCNodeDataPoolContent & rc_Content = this->c_ListValues[u32_ItConfig];
               const sint32 s32_DataSetIndex = this->c_DataSetSelectionIndices[u32_ItConfig];
               //List match
               if (((rc_Config.c_ElementId.u32_NodeIndex == orc_ListId.u32_NodeIndex) &&
                    (rc_Config.c_ElementId.u32_DataPoolIndex == orc_ListId.u32_DataPoolIndex)) &&
                   (rc_Config.c_ElementId.u32_ListIndex == orc_ListId.u32_ListIndex))
               {
                  if (s32_DataSetIndex >= 0)
                  {
                     //No error expected
                  }
                  else
                  {
                     if (rc_Config.c_ElementId.u32_ElementIndex < pc_List->c_Elements.size())
                     {
                        bool q_Ok;
                        const C_OSCNodeDataPoolListElement & rc_Element =
                           pc_List->c_Elements[rc_Config.c_ElementId.u32_ElementIndex];
                        s32_Retval = C_PuiSvDbParam::h_CheckMinMax(rc_Element, rc_Content, q_Ok);
                        if (s32_Retval == C_NO_ERR)
                        {
                           if (q_Ok == false)
                           {
                              orq_Error = true;
                           }
                        }
                     }
                     else
                     {
                        s32_Retval = C_RANGE;
                     }
                  }
               }
            }
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if value in range

   \param[in]  orc_Elem  Element
   \param[in]  orc_Value Value
   \param[out] orq_Ok    Flag if OK

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     21.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvDbParam::h_CheckMinMax(const C_OSCNodeDataPoolListElement & orc_Elem,
                                     const C_OSCNodeDataPoolContent & orc_Value, bool & orq_Ok)
{
   sint32 s32_Retval = C_NO_ERR;

   orq_Ok = false;
   //Check if comparable
   if ((((orc_Elem.c_MinValue.GetType() == orc_Value.GetType()) &&
         (orc_Elem.c_MaxValue.GetType() == orc_Value.GetType())) &&
        ((orc_Elem.c_MinValue.GetArray() == orc_Value.GetArray()) &&
         (orc_Elem.c_MaxValue.GetArray() == orc_Value.GetArray()))) &&
       ((orc_Elem.c_MinValue.GetArraySize() == orc_Value.GetArraySize()) &&
        (orc_Elem.c_MaxValue.GetArraySize() == orc_Value.GetArraySize())))
   {
      //Actual error check
      if ((orc_Elem.c_MinValue <= orc_Value) && (orc_Elem.c_MaxValue >= orc_Value))
      {
         //Content as expected
         orq_Ok = true;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}
