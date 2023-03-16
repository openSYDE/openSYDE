//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI line arrow data: stores UI information (implementation)

   UI line arrow data: stores UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "constants.hpp"
#include "C_PuiBsLineArrow.hpp"

#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::scl;

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
   c_UiColorDark(mc_STYLE_GUIDE_COLOR_36),
   e_StartArrowHeadType(eNONE),
   e_EndArrowHeadType(eNONE),
   e_LineType(eSOLID)
{
   c_UiColor = mc_STYLE_GUIDE_COLOR_7;
   s32_UiWidthPixels = 2;
   f64_ZetOrder = 0.0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsLineArrow::CalcHash(uint32_t & oru32_HashValue) const
{
   int32_t s32_Value;

   C_SclChecksums::CalcCRC32(&this->e_StartArrowHeadType, sizeof(this->e_StartArrowHeadType), oru32_HashValue);
   C_SclChecksums::CalcCRC32(&this->e_EndArrowHeadType, sizeof(this->e_EndArrowHeadType), oru32_HashValue);
   C_SclChecksums::CalcCRC32(&this->e_LineType, sizeof(this->e_LineType), oru32_HashValue);

   s32_Value = this->c_UiColorDark.red();
   stw::scl::C_SclChecksums::CalcCRC32(&s32_Value, sizeof(s32_Value), oru32_HashValue);
   s32_Value = this->c_UiColorDark.green();
   stw::scl::C_SclChecksums::CalcCRC32(&s32_Value, sizeof(s32_Value), oru32_HashValue);
   s32_Value = this->c_UiColorDark.blue();
   stw::scl::C_SclChecksums::CalcCRC32(&s32_Value, sizeof(s32_Value), oru32_HashValue);
   s32_Value = this->c_UiColorDark.alpha();
   stw::scl::C_SclChecksums::CalcCRC32(&s32_Value, sizeof(s32_Value), oru32_HashValue);

   C_PuiBsLineBase::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get arrow head type as string for file save.

   \param[in]  ore_Type    Arrow head type

   \return
   Arrow head type as string
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_PuiBsLineArrow::h_ArrowHeadTypeToString(const E_ArrowHeadType & ore_Type)
{
   stw::scl::C_SclString c_Retval;
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
/*! \brief   Get line type as string for file save.

   \param[in]  ore_Type    Line type

   \return
   Line type as string
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_PuiBsLineArrow::h_LineTypeToString(const E_LineType & ore_Type)
{
   stw::scl::C_SclString c_Retval;
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
/*! \brief   Get arrow head type from string for file load.

   \param[in]  orc_Str  Arrow type as string

   \return
   Arrow type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsLineArrow::E_ArrowHeadType C_PuiBsLineArrow::h_ArrowHeadTypeFromString(const stw::scl::C_SclString & orc_Str)
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
/*! \brief   Get line head type from string for file load.

   \param[in]  orc_Str  Line type as string

   \return
   Line type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsLineArrow::E_LineType C_PuiBsLineArrow::h_LineTypeFromString(const stw::scl::C_SclString & orc_Str)
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
