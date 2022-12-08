//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table Widget for Node COM Interface Settings (header)

   Table Widget for Node COM Interface Settings
   (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETABLEWIDGETCOMIF_H
#define C_OGETABLEWIDGETCOMIF_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>
#include <QTableWidget>
#include "stwtypes.hpp"
#include "C_NagToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OgeTableWidgetComIf :
   public QTableWidget
{
   Q_OBJECT

public:
   C_OgeTableWidgetComIf(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeTableWidgetComIf(void);

   void SetToolTipAt(const uint32_t ou32_Row, const uint32_t ou32_Col, const QString & orc_Heading,
                     const QString & orc_Content,
                     const stw::opensyde_gui::C_NagToolTip::E_Type oe_Type = stw::opensyde_gui::C_NagToolTip::eDEFAULT);
   void SetToolTipHeadingAt(const uint32_t ou32_Section, const Qt::Orientation oe_Orientation,
                            const QString & orc_Heading, const QString & orc_Content) const;

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

   virtual void m_HideToolTip(void);

private:
   class C_ItemId
   {
   public:
      int32_t s32_Row;
      int32_t s32_Col;

      bool operator <(const C_ItemId & orc_Item) const;
   };

   class C_ToolTipContent
   {
   public:
      QString c_Heading;
      QString c_Content;
      stw::opensyde_gui::C_NagToolTip::E_Type e_Type;
   };

   std::map<C_ItemId, C_ToolTipContent> mc_ToolTips;
   stw::opensyde_gui::C_NagToolTip * mpc_ToolTip;
   C_ItemId mc_HoveredRow;
   int32_t ms32_HoveredVertHeader;
   int32_t ms32_HoveredHorzHeader;

   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
   void m_ScrollBarRangeChangedVer(const int32_t os32_Min, const int32_t os32_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
