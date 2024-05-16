//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for update properties
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAUPPROPERTIES_HPP
#define C_FLAUPPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QIcon>
#include <QVBoxLayout>

#include "C_FlaUpListWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaUpProperties;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaUpProperties :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_FlaUpProperties(QWidget * const opc_Parent = NULL);
   ~C_FlaUpProperties(void) override;

   void SaveUserSettings(void) const;
   QStringList GetHexFilePaths(void) const;
   QStringList GetHexFileDeviceNames(void) const;
   bool AreAllFilesValid(void) const;
   void EnableSettings(const bool oq_Enabled);
   void ResetSummary(void);
   void SetHeadingText(const QString & orc_Text, const uint8_t & oru8_ProgressState) const;
   void UpdateProgressBar(const uint32_t & oru32_ProgressInPercentage, const bool oq_Finished = false) const;
   void SetElapsedTime(const QString & orc_Text) const;
   void UpdateDataTransfer(void) const;
   void UpdateDataTransfer(const uint64_t & oru64_FlashedBytes) const;
   void SetStatusIcon(const uint32_t & oru32_FlashedFiles, const uint8_t & oru8_State = 0U);
   void ResetStatusIcons(void);
   int32_t GetHexFileSize(void) const;
   uint32_t GetHexFileSize(const uint32_t & oru32_HexFileIndex) const;
   void SetHeadingIcon(const uint8_t & oru8_ProgressState) const;
   void SetProgressBarColor(const bool & orq_Success) const;

protected:
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;

private:
   void m_UpdateLabelTitleWithFileCounter(void);
   void m_FileAdded(void);

   Ui::C_FlaUpProperties * mpc_Ui;
   QVBoxLayout * mpc_ScrollLayout;
   C_FlaUpListWidget * mpc_ListWidget;

   //Avoid call
   C_FlaUpProperties(const C_FlaUpProperties &);
   C_FlaUpProperties & operator =(const C_FlaUpProperties &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
