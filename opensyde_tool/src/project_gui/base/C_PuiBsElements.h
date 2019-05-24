//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common scene elements (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSELEMENTS_H
#define C_PUIBSELEMENTS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_PuiBsBoundary.h"
#include "C_PuiBsImage.h"
#include "C_PuiBsLineArrow.h"
#include "C_PuiBsTextElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsElements
{
public:
   C_PuiBsElements(void);
   C_PuiBsElements & operator =(const C_PuiBsElements & orc_Snapshot);
   virtual ~C_PuiBsElements(void);
   virtual void Clear(void);
   void ReplaceSnapshotElements(C_PuiBsElements & orc_Snapshot) const;
   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   virtual stw_types::uint32 Count(void) const;

   //Add
   void AddBoundary(const C_PuiBsBoundary & orc_Data);
   stw_types::sint32 InsertBoundary(const stw_types::uint32 & oru32_Index, const C_PuiBsBoundary & orc_Data);
   void AddImage(const C_PuiBsImage & orc_Data);
   stw_types::sint32 InsertImage(const stw_types::uint32 & oru32_Index, const C_PuiBsImage & orc_Data);
   void AddLineArrow(const C_PuiBsLineArrow & orc_Data);
   stw_types::sint32 InsertLineArrow(const stw_types::uint32 & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   void AddTextElement(const C_PuiBsTextElement & orc_Data);
   stw_types::sint32 InsertTextElement(const stw_types::uint32 & oru32_Index, const C_PuiBsTextElement & orc_Data);

   //Delete
   stw_types::sint32 DeleteBoundary(const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteImage(const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteLineArrow(const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteTextElement(const stw_types::uint32 & oru32_Index);

   //Setter and getter
   const C_PuiBsBoundary * GetBoundary(const stw_types::uint32 & oru32_Index) const;
   stw_types::sint32 SetBoundary(const stw_types::uint32 & oru32_Index, const C_PuiBsBoundary & orc_Data);
   const C_PuiBsImage * GetImage(const stw_types::uint32 & oru32_Index) const;
   stw_types::sint32 SetImage(const stw_types::uint32 & oru32_Index, const C_PuiBsImage & orc_Data);
   const C_PuiBsLineArrow * GetLineArrow(const stw_types::uint32 & oru32_Index) const;
   stw_types::sint32 SetLineArrow(const stw_types::uint32 & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   const C_PuiBsTextElement * GetTextElement(const stw_types::uint32 & oru32_Index) const;
   stw_types::sint32 SetTextElement(const stw_types::uint32 & oru32_Index, const C_PuiBsTextElement & orc_Data);

   std::vector<C_PuiBsBoundary> c_Boundaries;      ///< UI information for boundaries
   std::vector<C_PuiBsImage> c_Images;             ///< UI information for images
   std::vector<C_PuiBsLineArrow> c_LineArrows;     ///< UI information for line arrows
   std::vector<C_PuiBsTextElement> c_TextElements; ///< UI information for text elements
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
