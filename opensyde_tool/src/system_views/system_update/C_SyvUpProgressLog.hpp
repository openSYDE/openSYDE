//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for progress log (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPROGRESSLOG_H
#define C_SYVUPPROGRESSLOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>
#include <QWidget>
#include "stwtypes.hpp"
#include "C_SyvUpProgressLogEntry.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpProgressLog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvUpProgressLog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpProgressLog(QWidget * const opc_Parent = NULL);
   ~C_SyvUpProgressLog(void) override;

   void Clear(void);
   void SetHeading(const QString & orc_Value) const;
   void AddSubHeading(const QString & orc_Value);
   void AddEntry(const uint32_t ou32_NodeIndex, const QString & orc_Value, const bool oq_Update);
   void AddSpacer(void);
   void UpdateStatus(const uint32_t ou32_NodeIndex, const QString & orc_Value, const bool oq_Update) const;
   void AddLogHyperlink(void);
   std::vector<uint32_t> GetConnectNodeEntryIndices(void);

private:
   Ui::C_SyvUpProgressLog * mpc_Ui;
   std::vector<C_SyvUpProgressLogEntry *> mc_ConnectEntries;
   std::vector<C_SyvUpProgressLogEntry *> mc_SpacerEntries;
   std::vector<C_SyvUpProgressLogEntry *> mc_UpdateEntries;
   std::vector<QLabel *> mc_ProcessEntries;
   QLabel * mpc_LogHyperlink;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;

   void m_SetHeadingVisible(void) const;
   void m_OpenLink(void) const;
   void m_ScrollToBottom(const int32_t os32_Min, const int32_t os32_Max) const;
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_CopyHyperLink() const;

   //Avoid call
   C_SyvUpProgressLog(const C_SyvUpProgressLog &);
   C_SyvUpProgressLog & operator =(const C_SyvUpProgressLog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
