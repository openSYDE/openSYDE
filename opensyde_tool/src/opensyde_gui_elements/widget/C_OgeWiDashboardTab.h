//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for dashboard tabs (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWIDASHBOARDTAB_H
#define C_OGEWIDASHBOARDTAB_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QAction>
#include "stwtypes.h"
#include "C_OgeContextMenu.h"

namespace Ui
{
class C_OgeWiDashboardTab;
}
/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_OgeWiDashboardTab :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiDashboardTab(QWidget * const opc_Parent = NULL, const bool oq_ShowUndock = false);
   ~C_OgeWiDashboardTab(void);
   void SetText(const QString & orc_Text);
   void SetToolTip(const QString & orc_Heading, const QString & orc_Content) const;
   void SetPinned(const bool oq_Value);
   void SetCurrent(const bool oq_Value);
   void SetActive(const bool oq_Value);
   void SetCloseButtonVisibility(const bool oq_Visibility);
   void DeactivateAdditionalActions(void);
   void SetDarkMode(const bool oq_Active);
   void SetInteractive(const bool oq_Active);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigActiveChanged(C_OgeWiDashboardTab * const opc_Source, const bool oq_Active);
   void SigUndockAction(C_OgeWiDashboardTab * const opc_Source);
   void SigCopyAction(C_OgeWiDashboardTab * const opc_Source);
   void SigCutAction(C_OgeWiDashboardTab * const opc_Source);
   void SigPasteAction(void);
   void SigDeleteAction(C_OgeWiDashboardTab * const opc_Source);
   void SigCloseAction(C_OgeWiDashboardTab * const opc_Source);
   void SigEditPropertiesAction(C_OgeWiDashboardTab * const opc_Source);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_OgeWiDashboardTab * mpc_Ui;
   QAction * mpc_ActionActivate;
   QAction * mpc_ActionCut;
   QAction * mpc_ActionPaste;
   QAction * mpc_ActionDelete;
   QAction * mpc_ActionClose;
   bool mq_Pinned;
   bool mq_Current;
   bool mq_Active;
   bool mq_EditActive;
   bool mq_Interactive;
   bool mq_FinishRenamingActive;
   bool mq_CloseButtonVisible;
   const bool mq_ShowUndock;
   stw_opensyde_gui_elements::C_OgeContextMenu mc_ContextMenu;

   void m_HandleMode(void);
   void m_InitContextMenu(void);
   void m_ToggleActive(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_EditProperties(void);
   void m_CopyAction(void);
   void m_CutAction(void);
   void m_PasteAction(void);
   void m_DeleteAction(void);
   void m_CloseAction(void);
   void m_UndockAction(void);
   //Avoid call
   C_OgeWiDashboardTab(const C_OgeWiDashboardTab &);
   C_OgeWiDashboardTab & operator =(const C_OgeWiDashboardTab &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
