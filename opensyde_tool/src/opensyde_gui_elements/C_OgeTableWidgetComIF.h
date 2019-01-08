//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Table Widget for Node COM Interface Settings (header)

   Table Widget for Node COM Interface Settings
   (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETABLEWIDGETCOMIF_H
#define C_OGETABLEWIDGETCOMIF_H

/* -- Includes ------------------------------------------------------------- */
#include <map>
#include <QTableWidget>
#include "stwtypes.h"
#include "C_NagToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_OgeTableWidgetComIF :
   public QTableWidget
{
   Q_OBJECT

public:
   C_OgeTableWidgetComIF(QWidget * const opc_Parent = NULL);
   ~C_OgeTableWidgetComIF(void);

   void SetToolTipAt(const stw_types::uint32 ou32_Row, const stw_types::uint32 ou32_Col, const QString & orc_Heading,
                     const QString & orc_Content,
                     const stw_opensyde_gui::C_NagToolTip::E_Type oe_Type = stw_opensyde_gui::C_NagToolTip::eDEFAULT);
   void SetToolTipHeadingAt(const stw_types::uint32 ou32_Section, const Qt::Orientation oe_Orientation,
                            const QString & orc_Heading, const QString & orc_Content) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   class C_ItemID
   {
   public:
      stw_types::sint32 s32_Row;
      stw_types::sint32 s32_Col;

      bool operator <(const C_ItemID & orc_Item) const;
   };

   class C_ToolTipContent
   {
   public:
      QString c_Heading;
      QString c_Content;
      stw_opensyde_gui::C_NagToolTip::E_Type e_Type;
   };

   std::map<C_ItemID, C_ToolTipContent> mc_ToolTips;
   stw_opensyde_gui::C_NagToolTip * mpc_ToolTip;
   C_ItemID mc_HoveredRow;
   stw_types::sint32 ms32_HoveredVertHeader;
   stw_types::sint32 ms32_HoveredHorzHeader;

   void m_HideTooltip(void);
   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
   void m_ScrollBarRangeChangedVer(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
