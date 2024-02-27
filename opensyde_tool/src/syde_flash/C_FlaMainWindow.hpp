//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main window for project SYDEFlash
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAMAINWINDOW_HPP
#define C_FLAMAINWINDOW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMainWindow>
#include <QTimer>

#include "C_FlaSenDcBasicSequences.hpp"
#include "C_FlaUpSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaMainWindow;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaMainWindow :
   public QMainWindow
{
   Q_OBJECT

public:
   explicit C_FlaMainWindow(QWidget * const opc_Parent = NULL);
   ~C_FlaMainWindow(void) override;

protected:
   void closeEvent(QCloseEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_FlaMainWindow * mpc_Ui;

   static const int32_t mhs32_SETTINGS_SPLITTER_MAX;

   // For custom splitter resizing
   int32_t ms32_SettingsSplitterPosition;

   //Avoid call
   C_FlaMainWindow(const C_FlaMainWindow &);
   C_FlaMainWindow & operator =(const C_FlaMainWindow &) &;

   void m_LoadUserSettings(void);
   void m_SaveUserSettings(void);

   void m_OnExpandSettings(const bool oq_Expand);
   void m_OnSettingsSplitterHandleDoubleClick(void) const;
   void m_OnSettingsSplitterMoved(const int32_t & ors32_Pos, const int32_t & ors32_Index);

   void m_OnUpdateNode(void);
   void m_OnSearchNode(void);
   void m_OnConfigureNode(void);
   void m_ShowProgress(const QString & orc_Text);

   int32_t m_InitUpdateSequence(void);
   void m_CleanupUpdateSequence(void);
   void m_TimerUpdate(void);
   void m_CheckDeviceName(const QString & orc_ReadDeviceName);

   stw::opensyde_gui_logic::C_FlaUpSequences * mpc_UpSequences;
   QTimer mc_TimerUpdate;
   stw::opensyde_gui_logic::C_FlaUpSequences::E_Sequence me_UpdateStep;
   int32_t ms32_NextHexFile;
   bool mq_ContinueUpdate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
