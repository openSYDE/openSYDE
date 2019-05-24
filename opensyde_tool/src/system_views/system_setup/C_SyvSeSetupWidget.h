//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view setup

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVSESETUPWIDGET_H
#define C_SYVSESETUPWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.h"

#include "C_SyvSeScene.h"
#include "C_PuiSvData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvSeSetupWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvSeSetupWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvSeSetupWidget(const stw_types::uint32 ou32_ViewIndex, QWidget * const opc_Parent = NULL);
   virtual ~C_SyvSeSetupWidget(void);

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
   void SigCheckViewError(const stw_types::uint32 ou32_ViewIndex) const;
   void SigEnableConfiguration(const bool oq_State) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvSeSetupWidget(const C_SyvSeSetupWidget &);
   C_SyvSeSetupWidget & operator =(const C_SyvSeSetupWidget &);

   void m_ViewChanged(void);
   void m_CheckViewForErrorGeneral(void) const;
   void m_CheckViewForError(const bool oq_SendError = true) const;
   void m_OnViewConnectionChange(void);

   void m_SelectAllStateChanged(void);
   void m_ConfirmClicked(void);
   void m_CancelClicked(void);

   Ui::C_SyvSeSetupWidget * mpc_Ui;

   C_SyvSeScene * mpc_Scene;
   stw_opensyde_gui_logic::C_PuiSvData mc_ViewCopy;

   const stw_types::uint32 mu32_ViewIndex;
   bool mq_EditModeActive;
   bool mq_IgnoreSelectAllCheckboxChanges;
};
}

#endif
