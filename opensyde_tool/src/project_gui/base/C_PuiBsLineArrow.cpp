//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI line arrow data: stores UI information (implementation)

   UI line arrow data: stores UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "constants.h"
#include "C_PuiBsLineArrow.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_scl;

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
C_PuiBsLineArrow::C_PuiBsLineArrow() :
   C_PuiBsLineBase(),
   c_UIColorDark(mc_STYLE_GUIDE_COLOR_36),
   e_StartArrowHeadType(eNONE),
   e_EndArrowHeadType(eNONE),
   e_LineType(eSOLID)
{
   c_UIColor = mc_STYLE_GUIDE_COLOR_7;
   s32_UIWidthPixels = 2;
   f64_ZOrder = 0.0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsLineArrow::CalcHash(uint32 & oru32_HashValue) const
{
   sintn sn_Value;

   C_SCLChecksums::CalcCRC32(&this->e_StartArrowHeadType, sizeof(this->e_StartArrowHeadType), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(&this->e_EndArrowHeadType, sizeof(this->e_EndArrowHeadType), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(&this->e_LineType, sizeof(this->e_LineType), oru32_HashValue);

   sn_Value = this->c_UIColorDark.red();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorDark.green();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorDark.blue();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);
   sn_Value = this->c_UIColorDark.alpha();
   stw_scl::C_SCLChecksums::CalcCRC32(&sn_Value, sizeof(sn_Value), oru32_HashValue);

   C_PuiBsLineBase::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Failsafe arrow head type to string

   \param[in] ore_Type Arrow head type
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_PuiBsLineArrow::ArrowHeadTypeToString(const E_ArrowHeadType & ore_Type)
{
   stw_scl::C_SCLString c_Retval;
   switch (ore_Type)
   {
   case eNORMAL:
      c_Retval = "normal";
      break;
   case eLINE:
      c_Retval = "line";
      break;
   case eRECT:
      c_Retval = "rect";
      break;
   case eCIRCLE:
      c_Retval = "circle";
      break;
   case eHALF:
      c_Retval = "half";
      break;
   case eNONE:
   default:
      c_Retval = "none";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Failsafe line type to string

   \param[in] ore_Type Line type
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_PuiBsLineArrow::LineTypeToString(const E_LineType & ore_Type)
{
   stw_scl::C_SCLString c_Retval;
   switch (ore_Type)
   {
   case eDASH:
      c_Retval = "dash";
      break;
   case eDOT:
      c_Retval = "dot";
      break;
   case eDASH_DOT:
      c_Retval = "dash-dot";
      break;
   case eDASH_DOT_DOT:
      c_Retval = "dash-dot-dot";
      break;
   case eSOLID:
   default:
      c_Retval = "solid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Failsafe arrow head type from string

   \param[in] orc_Str Stringified arrow head type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsLineArrow::E_ArrowHeadType C_PuiBsLineArrow::ArrowHeadTypeFromString(const stw_scl::C_SCLString & orc_Str)
{
   C_PuiBsLineArrow::E_ArrowHeadType e_Retval;
   if (orc_Str == "normal")
   {
      e_Retval = eNORMAL;
   }
   else if (orc_Str == "line")
   {
      e_Retval = eLINE;
   }
   else if (orc_Str == "rect")
   {
      e_Retval = eRECT;
   }
   else if (orc_Str == "circle")
   {
      e_Retval = eCIRCLE;
   }
   else if (orc_Str == "half")
   {
      e_Retval = eHALF;
   }
   else
   {
      e_Retval = eNONE;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Failsafe line type from string

   \param[in] orc_Str Stringified line type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsLineArrow::E_LineType C_PuiBsLineArrow::LineTypeFromString(const stw_scl::C_SCLString & orc_Str)
{
   C_PuiBsLineArrow::E_LineType e_Retval;
   if (orc_Str == "dash")
   {
      e_Retval = eDASH;
   }
   else if (orc_Str == "dot")
   {
      e_Retval = eDOT;
   }
   else if (orc_Str == "dash-dot")
   {
      e_Retval = eDASH_DOT;
   }
   else if (orc_Str == "dash-dot-dot")
   {
      e_Retval = eDASH_DOT_DOT;
   }
   else
   {
      e_Retval = eSOLID;
   }
   return e_Retval;
}
