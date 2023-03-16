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
#include "stwtypes.hpp"

#include "C_GiBiRectBaseGroup.hpp"
#include "C_GiRect.hpp"
#include "C_PuiBsBoundary.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiBoundary :
   public C_GiBiRectBaseGroup
{
public:
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiBoundary(const uint64_t & oru64_Id = 0U, const float64_t of64_Width = mhf64_MIN_WIDTH_BOUNDARY,
                  const float64_t of64_Height = mhf64_MIN_HEIGHT_BOUNDARY, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiBiBoundary(void) override;

   int32_t type() const override;
   bool OpenStyleDialog(const bool oq_DarkMode);
   void CopyStyle(const QGraphicsItem * const opc_GuidelineItem) override;

   void SetBorderWidth(const int32_t & ors32_Width);
   void SetBorderColor(const QColor & orc_Color);
   void SetBackgroundColor(const QColor & orc_Color);

   int32_t GetBorderWidth(void) const;
   QColor GetBorderColor(void) const;
   QColor GetBackgroundColor(void) const;

protected:
   void m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight) override;

   void m_LoadFromData(const stw::opensyde_gui_logic::C_PuiBsBoundary & orc_Data, const bool oq_DarkMode);
   void m_UpdateData(stw::opensyde_gui_logic::C_PuiBsBoundary & orc_Data, const bool oq_DarkMode) const;

   C_GiRect * mpc_Rectangle;

   static const float64_t mhf64_MIN_WIDTH_BOUNDARY;
   static const float64_t mhf64_MIN_HEIGHT_BOUNDARY;

private:
   //Avoid call
   C_GiBiBoundary(const C_GiBiBoundary &);
   C_GiBiBoundary & operator =(const C_GiBiBoundary &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
