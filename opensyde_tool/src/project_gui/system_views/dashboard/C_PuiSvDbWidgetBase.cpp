//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI base class for dashboard widget data (implementation)

   UI base class for dashboard widget data

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "C_PuiSvDbWidgetBase.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
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

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbWidgetBase::C_PuiSvDbWidgetBase(void) :
   C_PuiBsBox(),
   e_ElementWriteMode(eWM_MANUAL),
   e_DisplayStyle(eOPENSYDE)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbWidgetBase::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->c_DataPoolElementsConfig.size(); ++u32_Counter)
   {
      this->c_DataPoolElementsConfig[u32_Counter].CalcHash(oru32_HashValue);
   }
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_ElementWriteMode, sizeof(this->e_ElementWriteMode), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_DisplayStyle, sizeof(this->e_DisplayStyle), oru32_HashValue);

   C_PuiBsBox::CalcHash(oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current index for specifed element ID

   \param[in]  orc_Id      Data element id
   \param[out] oru32_Index Data element index

   \return
   C_NO_ERR Data element id found
   C_RANGE  Data element id not found

   \created     10.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiSvDbWidgetBase::GetIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_Id, uint32 & oru32_Index) const
{
   sint32 s32_Retval = C_RANGE;

   for (uint32 u32_It = 0; u32_It < this->c_DataPoolElementsConfig.size(); ++u32_It)
   {
      const C_PuiSvDbNodeDataElementConfig & rc_Config = this->c_DataPoolElementsConfig[u32_It];
      if (rc_Config.c_ElementId == orc_Id)
      {
         s32_Retval = C_NO_ERR;
         oru32_Index = u32_It;
      }
   }
   return s32_Retval;
}
