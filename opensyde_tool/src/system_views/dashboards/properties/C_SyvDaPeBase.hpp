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
#include "C_OgePopUpDialog.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_PuiSvDbDataElementScaling.hpp"
#include "C_PuiSvDbDataElementDisplayFormatter.hpp"
#include "C_SyvDaDashboardScene.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeBase;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeBase :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeBase(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32_t ou32_ViewIndex,
                          const uint32_t ou32_DashboardIndex, const QString & orc_Name,
                          const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                          const stw::opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_Scaling,
                          const bool oq_UseFormatterConfig,
                          const stw::opensyde_gui_logic::C_PuiSvDbDataElementDisplayFormatter & orc_FormatterConfig,
                          const bool oq_ReadElement, const bool oq_DarkMode,
                          const bool oq_ShowWidgetSpecificPart = true, const bool oq_AllowChangeOfDataElement = true,
                          const QString & orc_DisplayName = "");
   ~C_SyvDaPeBase(void) override;

   void InitStaticNames(void);
   void SetWidget(QWidget * const opc_Widget);
   void SetWriteMode(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode oe_WriteMode) const;
   void SetTheme(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style) const;
   C_SyvDaDashboardScene * GetPreviewScene(void);
   static QSize h_GetSceneViewSize(void);
   static float64_t h_GetFixIconOffset(void);
   static void h_GetIdealItemRect(QPointF & orc_ItemPos, QSizeF & orc_ItemSize, const QGraphicsItem * const opc_Item);
   stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId GetDataElementId(void) const;
   stw::opensyde_gui_logic::C_PuiSvDbDataElementScaling GetScalingInformation(void) const;
   stw::opensyde_gui_logic::C_PuiSvDbDataElementDisplayFormatter GetFormatterInformation(void) const;
   stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_WriteMode GetWriteMode(void) const;
   stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style GetTheme(void) const;
   QString GetDisplayName(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigRefresh(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SyvDaPeBase(const C_SyvDaPeBase &);
   C_SyvDaPeBase & operator =(const C_SyvDaPeBase &) &;

   Ui::C_SyvDaPeBase * mpc_Ui;
   stw::opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   C_SyvDaDashboardScene * mpc_Scene;
   stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_DataElement;
   const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_InitialDataElement;
   stw::opensyde_core::C_OscNodeDataPoolListElementId mc_OriginalConfigId;
   stw::opensyde_gui_logic::C_PuiSvReadDataConfiguration mc_OriginalConfigData;
   const uint32_t mu32_ViewIndex;
   const uint32_t mu32_DashboardIndex;
   bool mq_NewConfigAdded;
   bool mq_ConfigRestorationNecessary;
   const bool mq_ReadElement;
   const QString mc_DisplayName;
   static const int32_t mhs32_INDEX_MANUAL;
   static const int32_t mhs32_INDEX_ON_CHANGE;
   static const int32_t mhs32_INDEX_THEME_OPENSYDE;
   static const int32_t mhs32_INDEX_THEME_OPENSYDE_2;
   static const int32_t mhs32_INDEX_THEME_FLAT;
   static const int32_t mhs32_INDEX_THEME_SKEUMORPH;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_Browse(void);
   void m_Clear(void);
   void m_ClearDataElement(void);
   void m_Configuration(void) const;
   QString m_GetDefaultDisplayName(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id) const;
   void m_OnUseDefaultScalingChange(void) const;
   void m_OnFormatterActiveChange(void) const;

   void m_InitNoDataElement(void) const;
   void m_InitDataElement(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                          const stw::opensyde_gui_logic::C_PuiSvDbDataElementScaling & orc_Scaling,
                          const stw::opensyde_gui_logic::C_PuiSvDbDataElementDisplayFormatter & orc_FormatterConfig)
   const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
