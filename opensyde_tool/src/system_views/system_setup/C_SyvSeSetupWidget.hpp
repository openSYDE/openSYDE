//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view setup

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVSESETUPWIDGET_HPP
#define C_SYVSESETUPWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

#include "C_SyvSeScene.hpp"
#include "C_PuiSvData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvSeSetupWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvSeSetupWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvSeSetupWidget(const uint32_t ou32_ViewIndex, QWidget * const opc_Parent = NULL);
   ~C_SyvSeSetupWidget(void) noexcept override;

   void InitText() const;
   void Save(void) const;
   void LoadScene(void);
   void Abort(void);

   void SetEditMode(const bool oq_Active, const bool oq_WithSave = true);
   bool GetEditMode(void) const;
   void StartDeviceConfiguration(void);

   void PrepareToClose(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigCheckViewError(const uint32_t ou32_ViewIndex) const;
   void SigEnableConfiguration(const bool oq_State) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvSeSetupWidget(const C_SyvSeSetupWidget &);
   C_SyvSeSetupWidget & operator =(const C_SyvSeSetupWidget &) &;

   void m_ViewChanged(void);
   void m_CheckViewForErrorGeneral(void) const;
   void m_CheckViewForError(const bool oq_SendError = true) const;
   void m_OnViewConnectionChange(void);

   void m_SelectAllStateChanged(void);
   void m_ConfirmClicked(void);
   void m_CancelClicked(void);

   Ui::C_SyvSeSetupWidget * mpc_Ui;

   C_SyvSeScene * mpc_Scene;
   stw::opensyde_gui_logic::C_PuiSvData mc_ViewCopy;

   const uint32_t mu32_ViewIndex;
   bool mq_EditModeActive;
   bool mq_IgnoreSelectAllCheckboxChanges;
};
}
}

#endif
