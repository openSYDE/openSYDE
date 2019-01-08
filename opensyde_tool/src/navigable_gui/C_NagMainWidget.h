//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for main start view with use case buttons

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_NAGMAINWIDGET_H
#define C_NAGMAINWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_NagMainWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_NagMainWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagMainWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_NagMainWidget();

   void InitText(void) const;
   void OnSaveProjAs(void);
   void UpdateRecentProjects(void);
   void LoadProject(const QString & orc_FilePath);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode = 0,
                      const stw_types::uint32 ou32_Index = 0U, const QString & orc_Name = "",
                      const QString & orc_SubSubName = "", const stw_types::uint32 ou32_Flag = 0U);
   void SigBeforeOtherProjectLoad(void);
   void SigOtherProjectLoaded(const bool & orq_SwitchToLastKnownMode);
   void SigNewApplicationName(const QString & orc_Name);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_NagMainWidget(const C_NagMainWidget &);
   C_NagMainWidget & operator =(const C_NagMainWidget &);

   void m_SysDefClicked();
   void m_SysViewClicked();
   void m_AboutClicked();
   void m_InitIcons(void) const;
   void m_InitGUIElements() const;
   void m_UpdateCurrProjInfo(void);
   void m_OnNewProj(void);
   void m_OnOpenProj(void);
   void m_OnEdit(void) const;
   void m_OnEditFinished(void);
   void m_OnClear(void);
   void m_OnIndexClicked(const QModelIndex & orc_ModelIndex);

   Ui::C_NagMainWidget * mpc_Ui;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
