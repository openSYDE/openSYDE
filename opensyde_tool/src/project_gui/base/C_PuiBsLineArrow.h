//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI line arrow data: stores UI information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSLINEARROW_H
#define C_PUIBSLINEARROW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QColor>
#include <QPointF>
#include "stwtypes.h"
#include "C_PuiBsLineBase.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;

   QColor c_UIColorDark;
   E_ArrowHeadType e_StartArrowHeadType;
   E_ArrowHeadType e_EndArrowHeadType;
   E_LineType e_LineType;

   static stw_scl::C_SCLString ArrowHeadTypeToString(const E_ArrowHeadType & ore_Type);
   static stw_scl::C_SCLString LineTypeToString(const E_LineType & ore_Type);
   static E_ArrowHeadType ArrowHeadTypeFromString(const stw_scl::C_SCLString & orc_Str);
   static E_LineType LineTypeFromString(const stw_scl::C_SCLString & orc_Str);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
