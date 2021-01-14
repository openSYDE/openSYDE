//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard widget properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEBASE_H
#define C_SYVDAPEBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include "C_PuiSvReadDataConfiguration.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"
#include "C_PuiSvDbDataElementScaling.h"
#include "C_SyvDaDashboardScene.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeBase;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeBase :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeBase(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                          const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_DashboardIndex,
                          const QString & orc_Name = "", const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id =
                             stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                                                        stw_opensyde_gui_logic::
                                                                                        C_PuiSvDbNodeDataPoolListElementId
                                                                                        ::eDATAPOOL_ELEMENT, false, 0UL,
                                                                                        false), const stw_opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_Scaling =
                             stw_opensyde_gui_logic::C_PuiSvDbDataElementScaling(), const bool oq_ReadElement = true, const bool oq_DarkMode = false, const bool oq_ShowWidgetSpecificPart = true, const bool oq_AllowChangeOfDataElement = true,
                          const QString & orc_DisplayName = "");
   ~C_SyvDaPeBase(void);

   void InitStaticNames(void);
   void SetWidget(QWidget * const opc_Widget);
   void SetWriteMode(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode oe_WriteMode) const;
   void SetTheme(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style) const;
   C_SyvDaDashboardScene * GetPreviewScene(void);
   static QSize h_GetSceneViewSize(void);
   static stw_types::float64 h_GetFixIconOffset(void);
   static void h_GetIdealItemRect(QPointF & orc_ItemPos, QSizeF & orc_ItemSize, const QGraphicsItem * const opc_Item);
   stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId GetDataElementId(void) const;
   stw_opensyde_gui_logic::C_PuiSvDbDataElementScaling GetScalingInformation(void) const;
   stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode GetWriteMode(void) const;
   stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style GetTheme(void) const;
   QString GetDisplayName(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigRefresh(void);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SyvDaPeBase(const C_SyvDaPeBase &);
   C_SyvDaPeBase & operator =(const C_SyvDaPeBase &);

   Ui::C_SyvDaPeBase * mpc_Ui;
   stw_opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   C_SyvDaDashboardScene * mpc_Scene;
   stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_DataElement;
   const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_InitialDataElement;
   stw_opensyde_core::C_OSCNodeDataPoolListElementId mc_OriginalConfigId;
   stw_opensyde_gui_logic::C_PuiSvReadDataConfiguration mc_OriginalConfigData;
   const stw_types::uint32 mu32_ViewIndex;
   const stw_types::uint32 mu32_DashboardIndex;
   bool mq_NewConfigAdded;
   bool mq_ConfigRestorationNecessary;
   const bool mq_ReadElement;
   const QString mc_DisplayName;
   static const stw_types::sintn mhsn_INDEX_MANUAL;
   static const stw_types::sintn mhsn_INDEX_ON_CHANGE;
   static const stw_types::sintn mhsn_INDEX_THEME_OPENSYDE;
   static const stw_types::sintn mhsn_INDEX_THEME_OPENSYDE_2;
   static const stw_types::sintn mhsn_INDEX_THEME_FLAT;
   static const stw_types::sintn mhsn_INDEX_THEME_SKEUMORPH;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_Browse(void);
   void m_Clear(void);
   void m_ClearDataElement(void);
   void m_Configuration(void) const;
   QString m_GetDefaultDisplayName(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id) const;
   void m_OnUseDefaultScalingChange(void) const;

   void m_InitNoDataElement(void) const;
   void m_InitDataElement(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                          const stw_opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_Scaling) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
