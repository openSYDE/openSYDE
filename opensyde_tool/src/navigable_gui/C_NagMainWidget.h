//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for main start view with use case buttons

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_NAGMAINWIDGET_H
#define C_NAGMAINWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagMainWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagMainWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagMainWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_NagMainWidget();

   void InitText(void) const;
   void LoadInitialProject(void);
   void LoadProject(const QString & orc_FilePath);
   void UpdateRecentProjects(void);
   void OnSaveProjAs(void);
   void OnCreateServiceProj(void);
   void OpenColorPicker(void);
   void HandleServiceMode(void) const;

   static void h_GetFirstValidViewForServiceMode(stw_types::uint32 & oru32_ViewIndex, stw_types::sint32 & ors32_SubMode,
                                                 bool & orq_Found);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode = 0,
                      const stw_types::uint32 ou32_Index = 0U, const QString & orc_Name = "",
                      const QString & orc_SubSubName = "", const stw_types::uint32 ou32_Flag = 0U,
                      const bool oq_ChangeUseCase = false);
   void SigBeforeOtherProjectLoad(void);
   void SigOtherProjectLoaded(const bool & orq_SwitchToLastKnownMode);
   void SigNewApplicationName(const QString & orc_Name);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;

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
   void m_OnCreateServiceProj(void);
   void m_OnEdit(void) const;
   void m_OnEditFinished(void);
   void m_OnClear(void);
   void m_OnIndexClicked(const QModelIndex & orc_ModelIndex);
   stw_types::sint32 m_LoadConcreteProject(stw_types::uint16 * const opu16_FileVersion);
   stw_types::sint32 m_GetPassword(QString & orc_Password);
   void m_CancelPasswordDialog(stw_types::uint16 ou16_ProjectFileVersion);

   Ui::C_NagMainWidget * mpc_Ui;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
