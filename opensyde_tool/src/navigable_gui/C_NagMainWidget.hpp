//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for main start view with use case buttons

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_NAGMAINWIDGET_HPP
#define C_NAGMAINWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagMainWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagMainWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagMainWidget(QWidget * const opc_Parent = NULL);
   ~C_NagMainWidget() override;

   void InitText(void) const;
   void LoadInitialProject(void);
   void LoadProject(const QString & orc_FilePath);
   void UpdateRecentProjects(void);
   void OnSaveProjAs(void);
   void OnCreateServiceProj(void);
   void OpenColorPicker(void);
   void HandleServiceMode(void) const;

   static void h_GetFirstValidViewForServiceMode(uint32_t & oru32_ViewIndex, int32_t & ors32_SubMode, bool & orq_Found);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const int32_t os32_Mode, const int32_t os32_SubMode = 0, const uint32_t ou32_Index = 0U,
                      const QString & orc_Name = "", const QString & orc_SubSubName = "", const uint32_t ou32_Flag = 0U,
                      const bool oq_ChangeUseCase = false);
   void SigBeforeOtherProjectLoad(void);
   void SigOtherProjectLoaded(const bool & orq_SwitchToLastKnownMode);
   void SigNewApplicationName(const QString & orc_Name);
   void SigMaxCharLimitAccepted(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   //Avoid call
   C_NagMainWidget(const C_NagMainWidget &);
   C_NagMainWidget & operator =(const C_NagMainWidget &) &;

   void m_SysDefClicked();
   void m_SysViewClicked();
   void m_AboutClicked();
   void m_SettingsClicked();
   void m_InitIcons(void) const;
   void m_InitGuiElements() const;
   void m_UpdateCurrProjInfo(void);
   void m_OnNewProj(void);
   void m_OnOpenProj(void);
   void m_OnCreateServiceProj(void);
   void m_OnEdit(void) const;
   void m_OnEditFinished(void);
   void m_OnClear(void);
   void m_OnIndexClicked(const QModelIndex & orc_ModelIndex);
   int32_t m_LoadConcreteProject(uint16_t * const opu16_FileVersion);
   int32_t m_GetPassword(QString & orc_Password);
   void m_CancelPasswordDialog(uint16_t ou16_ProjectFileVersion);
   void m_SetNameStringLength(void);

   Ui::C_NagMainWidget * mpc_Ui;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
