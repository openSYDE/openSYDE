//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI line arrow data: stores UI information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSLINEARROW_HPP
#define C_PUIBSLINEARROW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QColor>
#include <QPointF>
#include "stwtypes.hpp"
#include "C_PuiBsLineBase.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsLineArrow :
   public C_PuiBsLineBase
{
public:
   enum E_ArrowHeadType
   {
      eNONE,
      eNORMAL,
      eLINE,
      eRECT,
      eCIRCLE,
      eHALF
   };

   enum E_LineType
   {
      eSOLID,
      eDASH,
      eDOT,
      eDASH_DOT,
      eDASH_DOT_DOT
   };

   C_PuiBsLineArrow();

   void CalcHash(uint32_t & oru32_HashValue) const override;

   QColor c_UiColorDark;
   E_ArrowHeadType e_StartArrowHeadType;
   E_ArrowHeadType e_EndArrowHeadType;
   E_LineType e_LineType;

   static stw::scl::C_SclString h_ArrowHeadTypeToString(const E_ArrowHeadType & ore_Type);
   static stw::scl::C_SclString h_LineTypeToString(const E_LineType & ore_Type);
   static E_ArrowHeadType h_ArrowHeadTypeFromString(const stw::scl::C_SclString & orc_Str);
   static E_LineType h_LineTypeFromString(const stw::scl::C_SclString & orc_Str);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
