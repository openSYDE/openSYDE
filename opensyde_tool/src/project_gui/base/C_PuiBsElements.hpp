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
#include "C_PuiBsBoundary.hpp"
#include "C_PuiBsImage.hpp"
#include "C_PuiBsLineArrow.hpp"
#include "C_PuiBsTextElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsElements
{
public:
   C_PuiBsElements(void);
   C_PuiBsElements & operator =(const C_PuiBsElements & orc_Snapshot) &;
   virtual ~C_PuiBsElements(void);
   virtual void Clear(void);
   void ReplaceSnapshotElements(C_PuiBsElements & orc_Snapshot) const;
   virtual void CalcHash(uint32_t & oru32_HashValue) const;
   virtual uint32_t Count(void) const;
   virtual void SetDataPositionOffset(const QPointF & orc_NewPos);
   virtual void SetDataZetOffset(const float64_t of64_HighestUsedZetValue);

   //Add
   void AddBoundary(const C_PuiBsBoundary & orc_Data);
   int32_t InsertBoundary(const uint32_t & oru32_Index, const C_PuiBsBoundary & orc_Data);
   void AddImage(const C_PuiBsImage & orc_Data);
   int32_t InsertImage(const uint32_t & oru32_Index, const C_PuiBsImage & orc_Data);
   void AddLineArrow(const C_PuiBsLineArrow & orc_Data);
   int32_t InsertLineArrow(const uint32_t & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   void AddTextElement(const C_PuiBsTextElement & orc_Data);
   int32_t InsertTextElement(const uint32_t & oru32_Index, const C_PuiBsTextElement & orc_Data);

   //Delete
   int32_t DeleteBoundary(const uint32_t & oru32_Index);
   int32_t DeleteImage(const uint32_t & oru32_Index);
   int32_t DeleteLineArrow(const uint32_t & oru32_Index);
   int32_t DeleteTextElement(const uint32_t & oru32_Index);

   //Setter and getter
   const C_PuiBsBoundary * GetBoundary(const uint32_t & oru32_Index) const;
   int32_t SetBoundary(const uint32_t & oru32_Index, const C_PuiBsBoundary & orc_Data);
   const C_PuiBsImage * GetImage(const uint32_t & oru32_Index) const;
   int32_t SetImage(const uint32_t & oru32_Index, const C_PuiBsImage & orc_Data);
   const C_PuiBsLineArrow * GetLineArrow(const uint32_t & oru32_Index) const;
   int32_t SetLineArrow(const uint32_t & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   const C_PuiBsTextElement * GetTextElement(const uint32_t & oru32_Index) const;
   int32_t SetTextElement(const uint32_t & oru32_Index, const C_PuiBsTextElement & orc_Data);

   std::vector<C_PuiBsBoundary> c_Boundaries;      ///< UI information for boundaries
   std::vector<C_PuiBsImage> c_Images;             ///< UI information for images
   std::vector<C_PuiBsLineArrow> c_LineArrows;     ///< UI information for line arrows
   std::vector<C_PuiBsTextElement> c_TextElements; ///< UI information for text elements
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
