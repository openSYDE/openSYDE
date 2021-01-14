//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element boundary (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIBOUNDARY_H
#define C_GIBOUNDARY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_GiBiRectBaseGroup.h"
#include "C_GiRect.h"
#include "C_PuiBsBoundary.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiBoundary :
   public C_GiBiRectBaseGroup
{
public:
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiBoundary(const stw_types::uint64 & oru64_ID = 0, const stw_types::float64 of64_Width = mhf64_MinWidthBoundary,
                  const stw_types::float64 of64_Height = mhf64_MinHeightBoundary,
                  QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiBiBoundary(void);

   virtual stw_types::sintn type() const override;
   bool OpenStyleDialog(const bool oq_DarkMode);
   virtual void CopyStyle(const QGraphicsItem * const opc_GuidelineItem) override;

   void SetBorderWidth(const stw_types::sint32 & ors32_Width);
   void SetBorderColor(const QColor & orc_Color);
   void SetBackgroundColor(const QColor & orc_Color);

   stw_types::sint32 GetBorderWidth(void) const;
   QColor GetBorderColor(void) const;
   QColor GetBackgroundColor(void) const;

protected:
   virtual void m_ResizeUpdateItems(const stw_types::float64 of64_DiffWidth,
                                    const stw_types::float64 of64_DiffHeight) override;

   void m_LoadFromData(const stw_opensyde_gui_logic::C_PuiBsBoundary & orc_Data, const bool oq_DarkMode);
   void m_UpdateData(stw_opensyde_gui_logic::C_PuiBsBoundary & orc_Data, const bool oq_DarkMode) const;

   C_GiRect * mpc_Rectangle;

   static const stw_types::float64 mhf64_MinWidthBoundary;
   static const stw_types::float64 mhf64_MinHeightBoundary;

private:
   //Avoid call
   C_GiBiBoundary(const C_GiBiBoundary &);
   C_GiBiBoundary & operator =(const C_GiBiBoundary &); //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
