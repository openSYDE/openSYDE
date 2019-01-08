//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for system view dashboards

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSWIDGET_H
#define C_SYVDADASHBOARDSWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QTimer>

#include "stwtypes.h"
#include "C_OgeWiHover.h"
#include "C_SyvComDriverDiag.h"
#include "C_SyvDaDashboardToolbox.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SyvDaDashboardsWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaDashboardsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardsWidget(const stw_types::uint32 ou32_ViewIndex, QWidget * const opc_ToolboxParent,
                                    QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardsWidget(void);

   void InitText() const;
   void LoadDarkMode(void);
   void Save(void) const;
   void OpenSettings(void);
   void ToggleDarkMode(void);

   void SetEditMode(const bool oq_Active);
   bool GetEditMode(void) const;

   bool GetConnectActive(void) const;
   stw_types::sint32 SetConnectActive(const bool oq_Value);

   void CheckError(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigEditModeClosed(void);
   void SigSetPushButtonIcon(const QString & orc_IconPath);
   void SigConfigError(const bool oq_Active);
   void SigBlockDragAndDrop(const bool oq_Block);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void hideEvent(QHideEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvDaDashboardsWidget(const C_SyvDaDashboardsWidget &);
   C_SyvDaDashboardsWidget & operator =(const C_SyvDaDashboardsWidget &);

   void m_ConfirmClicked(void);
   void m_CancelClicked(void);
   void m_ApplyDarkMode(const bool oq_Active);
   void m_InitToolBox(void);
   void m_CleanUpToolBox(void) const;
   void m_UpdateShowValues(void) const;

   stw_types::sint32 m_InitOsyDriver(QString & orc_Message);
   stw_types::sint32 m_InitNodes(QString & orc_Message, QString & orc_MessageDetails);
   void m_CloseOsyDriver(void);

   void m_DataPoolWrite(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                        const stw_types::uint16 ou16_ListIndex, const stw_types::uint16 ou16_ElementIndex);
   void m_DataPoolRead(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Index);
   void m_HandleManualOperationFinished(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC);

   Ui::C_SyvDaDashboardsWidget * mpc_Ui;
   stw_opensyde_gui_logic::C_SyvComDriverDiag * mpc_ComDriver;
   stw_opensyde_gui_elements::C_OgeWiHover * mpc_Toolbox;
   QWidget * const mpc_ToolboxParent;
   C_SyvDaDashboardToolbox * mpc_ToolboxContent;
   QTimer mc_Timer;
   stw_types::uint32 mu32_ViewIndex;
   bool mq_EditModeActive;
   bool mq_ConnectActive;
   std::set<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_MissedReadOperations;
   std::set<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_MissedWriteOperations;
   static stw_types::uint32 mhu32_DisconnectTime;
   static const QString mhc_DarkModeEnabledIconPath;
   static const QString mhc_DarkModeDisabledIconPath;
   static const stw_types::sintn mhsn_WidgetBorder;
   static const stw_types::sintn mhsn_ToolboxInitPosY;
};
}

#endif
