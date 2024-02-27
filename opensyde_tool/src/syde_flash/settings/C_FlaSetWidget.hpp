//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for settings section
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLASETWIDGET_HPP
#define C_FLASETWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_UsHandler.hpp"
#include "C_CamOgeWiSettingsBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaSetWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaSetWidget :
   public stw::opensyde_gui_elements::C_CamOgeWiSettingsBase
{
   Q_OBJECT

public:
   explicit C_FlaSetWidget(QWidget * const opc_Parent = NULL);
   ~C_FlaSetWidget(void) override;

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void ExpandSettings(const bool oq_Expand) const;

   QString GetCanDllPath(void) const;

   void ClearProgress(void);
   void ShowProgress(const QString & orc_Text);

   void EnableSettings(const bool oq_Enabled);

   int32_t GetFlashloaderResetWaitTime(void) const;
   int32_t GetRequestDownloadTimeout(void) const;
   int32_t GetTransferDataTimeout(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigExpandSettings(const bool oq_Expand);
   void SigCanDllConfigured(void);

protected:
   void showEvent(QShowEvent * const opc_Event) override;
   void m_PrepareInitialShow() const;

private:
   Ui::C_FlaSetWidget * mpc_Ui;

   stw::opensyde_gui::C_CamMosSectionPopup * mpc_PopupProgress;
   stw::opensyde_gui::C_CamMosSectionPopup * mpc_PopupDllConfig;
   stw::opensyde_gui::C_CamMosSectionPopup * mpc_PopupAdvSett;

   //Avoid call
   C_FlaSetWidget(const C_FlaSetWidget &);
   C_FlaSetWidget & operator =(const C_FlaSetWidget &) &;

   void m_OnExpandSettings(const bool oq_Expand);
   void m_HidePopupProgress(void) const;
   void m_HidePopupDllConfig(void) const;
   void m_HidePopupAdvSett(void) const;
   stw::opensyde_gui_logic::C_UsHandler::E_SettingsSubSection m_GetPopOpenIdentity(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
